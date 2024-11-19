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
int* heap_index;

void add_edge(int from, int to, int weight_main[], int period);
void dijkstra(int source, int SIZE, int period, int target);
void dequeue(tnode* arr, int n);
void update(tnode* arr, int i);

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
    heap_index = (int*) malloc(SIZE * sizeof(int));

    for(int i = 0; i < SIZE; i++) { // initialize heap and heap_index
        arr[i].label = i;
        arr[i].distance = INT_MAX;
        arr[i].predecessor = -1;
        arr[i].steps = 0;
        heap_index[i] = i;
    }
    arr[0].distance = 0;
    arr[0].label = source;
    arr[source].label = 0;
    heap_index[0] = source;
    heap_index[source] = 0;
    
    while (n != 0) {
        dequeue(arr, n - 1); // downward heapify for sorting δ(s, u), node w/ smallest distance
        n--;
        int u = arr[n].label; // u is current vertex label
        struct gnode* v = graph[u]; // v is adjacency list of u
        printf("We are visiting: %d\n", u);

        while (v != NULL) {
            // step weight
            int weight_index = 0;//arr[heap_index[u]].steps % period;
            int weight = v->weights[weight_index];
            printf("The weight from %d to %d is: %d\n", u, v->label, weight);

            if (heap_index[v->label] < n && // remainings in heap
            arr[heap_index[v->label]].distance > arr[heap_index[u]].distance + weight) {
                arr[heap_index[v->label]].distance = arr[heap_index[u]].distance + weight; // update distance w/ step weight
                arr[v->label].predecessor = u;
                //printf("UPDATED predecessor: Node %d -> %d\n", v->label, u);
                arr[heap_index[v->label]].steps = arr[heap_index[u]].steps + 1;
                update(arr, heap_index[v->label]); //upward heapify
            }
            v = v->next; // move to adjacent
        }
    }

    // for (int l = 0; l <SIZE; l++){
    //     printf("ARR %d %d\n", l, arr[l].label);
    //     printf("HI %d", heap_index[l]);
    // }

    int path[25];
    int path_length = 0;
    int current = target;
    while (current != -1) {
        path[path_length++] = current;
        printf("CURRENT: %d\n", current);
        current = arr[current].predecessor;
    }

    // for (int i = path_length - 1; i >= 0; i--) {
    //     printf("BLAH%d", path[i]);
    //     if (i > 0) printf(" ");
    // }
    // printf("\n");

    free(arr);
    free(heap_index);
}

void dequeue(tnode* arr, int n) { // n is the last index
    tnode temp = arr[n]; // exchange the root and the last node
    arr[n] = arr[0];
    arr[0] = temp;
    heap_index[arr[0].label] = 0; // swap the nodes in heap_index
    heap_index[arr[n].label] = n;
    n--; // decrease the heap size
    int i = 0, j;
    while ((j = 2*i+1) <= n) { // while left child exists
        if (j < n && arr[j].distance > arr[j+1].distance) // if right child smaller
        j = j+1; // pick right child
        if (temp.distance <= arr[j].distance) break; // if parent smaller than child, done
        else { // move small child
            arr[i] = arr[j];
            heap_index[arr[i].label] = i; // Update heap_index for the moved node
            i = j;
        }
    }
    arr[i] = temp;
    heap_index[temp.label] = i; // Update heap_index for the final position
    for (int k = 0; k <= n; k++){
        printf("The order of nodes is: %d indexed at %d w/ distance %d\n", 
        arr[k].label, heap_index[arr[k].label], arr[k].distance);
    }
    printf("DEQUEUED_______________\n");
}

void update(struct tnode* arr, int i)
{
    //Don’t forget to update heap_index
    // upward heapify
    while (i > 0 && arr[i].distance < arr[(i - 1) / 2].distance) {
        // Swap with parent
        int parent = (i - 1) / 2;
        tnode temp = arr[i];
        arr[i] = arr[parent];
        arr[parent] = temp;

        // Update heap_index to reflect new positions
        heap_index[arr[i].label] = parent;
        heap_index[arr[parent].label] = i;

        i = parent;
    }
    for (int k = 0; k < 4; k++){
        printf("The order of nodes is: %d indexed at %d w/ distance %d\n", 
        arr[k].label, heap_index[arr[k].label], arr[k].distance);
    }
    printf("UPDATED_______________\n");
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
    for (int g = 0; g < vertices; g++) {
        graph[g] = NULL;
    }
    
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
    while (scanf("%d %d", &user_start, &user_target) == 2) {
        // find shortest path from user_start to user_target
        //printf("%d %d heyyyy\n", user_start, user_target);
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
