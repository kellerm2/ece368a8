#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

typedef struct gnode {
    int label;
    int* weights;
    struct gnode* next;
} gnode;

typedef struct tnode {
    int label;
    int distance;
    int predecessor;
    int steps;
} tnode;

gnode** graph;

void add_edge(int from, int to, int weight_main[], int period);
void dijkstra(int source, int SIZE, int period, int target);
void dequeue(tnode* arr, int n);
void update(tnode* arr, int i, int heap_index[]);

void add_edge(int from, int to, int weight_main[], int period) {
    gnode* new = (struct gnode*) malloc(sizeof(struct gnode));
    new->label = to;
    new->weights = (int*) malloc(period * sizeof(int));
    for (int i = 0; i < period; i++) {
        new->weights[i] = weight_main[i];
    }
    new->next = graph[from];
    graph[from] = new;
}

void dijkstra(int source, int SIZE, int period, int target) {
    int n = SIZE; // n nodes
    struct tnode* arr = (struct tnode*) malloc(SIZE * sizeof(struct tnode));
    int* heap_index = (int*) malloc(SIZE * sizeof(int));
    for(int i = 0; i < SIZE; i++) { // initialize heap and heap_index
        arr[i].label = i;
        arr[i].distance = INT_MAX;
        arr[i].predecessor = -1;
        arr[i].steps = 0;
        heap_index[i] = i;
    }
    arr[0].distance = 0; // initialize start node as dist 0
    arr[0].label = source;
    arr[source].steps = 0;
    arr[source].label = 0;
    heap_index[0] = source;
    heap_index[source] = 0;
    
    while (n != 0) {
        dequeue(arr, n - 1); // downward heapify for sorting δ(s, u)
        n--;
        int u = arr[n].label; // u is current vertex label
        struct gnode* v = graph[u]; // v is adjacency list of u
        while (v != NULL) {
            int weight = v->weights[arr[heap_index[u]].steps % period]; // step weight
            if (heap_index[v->label] < n && // remainings in heap
            arr[heap_index[v->label]].distance >
            arr[heap_index[u]].distance + weight) {
                arr[heap_index[v->label]].distance =
                arr[heap_index[u]].distance + weight; // update distance w/ step weight
                arr[heap_index[v->label]].predecessor = u;
                arr[heap_index[v->label]].steps = arr[heap_index[u]].steps + 1;
                update(arr, heap_index[v->label], heap_index); //upward heapify
            }
            v = v->next; // move to adjacent
        }
    }
    int pred_list[25]; //CHANGE LATERRRR
    int k = 0;
    while ((arr[heap_index[target]].predecessor != -1)
            && (target != source)) {  
        pred_list[k] = target;
        target = arr[heap_index[target]].predecessor;
        k++;
    }
    pred_list[k] = source; // add source to end of list
    // list now in reverse order
    for (int l = k; l >= 0; l--) {
        if (l != 0) printf("%d ", pred_list[l]);
        else printf("%d", pred_list[l]);
    }
    printf("\n");

    free(arr);
    free(heap_index);
}

void dequeue(tnode* arr, int n) { // n is the last index
    tnode temp = arr[n]; // exchange the root and the last node
    arr[n] = arr[0];
    arr[0] = temp;
    n--; // decrease the heap size
    int i = 0, j;
    while ((j = 2*i+1) <= n) { // while left child exists
        if (j < n && arr[j].distance > arr[j+1].distance) // if right child smaller
        j = j+1; // pick right child
        if (temp.distance <= arr[j].distance) break; // if parent smaller than child, done
        else { // move small child
            arr[i] = arr[j];
            i = j;
        }
    }
    arr[i] = temp;
}

void update(struct tnode* arr, int i, int heap_index[])
{
    //Don’t forget to update heap_index
    tnode temp = arr[i];

    while (i > 0) {
        int parent = (i - 1) / 2; // parent index
        if (arr[parent].distance <= temp.distance) // if parent distance less, done
            break;

        // move parent down if parent has greater distance
        arr[i] = arr[parent];
        heap_index[arr[parent].label] = i;
        i = parent;
    }
    arr[i] = temp;
    heap_index[temp.label] = i; // update heap_index
}

int main(int argc, char* argv[]) {
    if (argc != 2) return 0; // args ./a6, file

    char* in_file = argv[1];
    FILE* file = (FILE*) fopen(in_file, "r"); // open txt file
    assert(file != NULL);

    // V N (# of vertices, period)
    // vs vt w1 . . . wN 
    int vertices;
    int period;
    assert(fscanf(file, "%d %d", &vertices, &period) == 2);
    
    graph = (gnode**) malloc(vertices * sizeof(gnode*));
    assert(graph != NULL);
    
    int vs;
    int vt;
    
    while (fscanf(file, "%d %d", &vs, &vt) != EOF) {  
        int* weight_main = (int*) malloc(period * sizeof(int));
        for (int i = 0; i < period; i++) {
            assert(fscanf(file, "%d", &weight_main[i]) == 1);
        }
        //make graph node w this
        add_edge(vs, vt, weight_main, period);
    }
    fclose(file);


    // for (int p = 0; p < vertices; p++) {
    //     gnode* curr = graph[p];
    //     while (curr) {
    //         printf("from %d to %d", p, curr->label);
    //         for (int t = 0; t < period; t++) {
    //             printf(" %d", curr->weights[t]);
    //         }
    //         printf("\n");
    //         curr = curr->next;
    //     }
    // }
    
    int user_start;
    int user_target;
    while (scanf("%d %d", &user_start, &user_target)) {
        // find shortest path from user_start to user_target
        dijkstra(user_start, vertices, period, user_target);
    }

    // free weights alloc, gnode alloc, graph
    for (int j = 0; j < vertices; j++) {
        gnode* current = graph[j];
        while (current != NULL) {
            gnode* temp = current;
            current = current->next;
            free(temp->weights);
            free(temp);
        }
    }
    free(graph);

    return 0;
}