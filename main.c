#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

typedef struct gnode {
    int label;
    int* weights;
    struct gnode* next;
    int steps;
} gnode;

typedef struct tnode {
    int label;
    int distance;
    int predecessor;
} tnode;

gnode** graph;

void add_edge(int from, int to, int weights[], int period);
void dijkstra(int source, int SIZE, int period, int target);
void dequeue(int* arr, int n);
void update(struct tnode* arr, int i);

void add_edge(int from, int to, int weights[], int period) {
    gnode* new = (struct gnode*) malloc(sizeof(struct gnode));
    new->label = to;
    new->weights = (int*) malloc(period * sizeof(int));
    for (int i = 0; i < period; i++) {
        new->weights[i] = weights[i];
    }
    new->next = graph[from];
    graph[from] = new;
}

void dijkstra(int source, int SIZE, int period, int target) {
    struct tnode arr[SIZE]; // heap arr size of nodes, static
    int n = SIZE; // n nodes
    int heap_index[SIZE]; // heap_index, static
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
    
    int step = 0;
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
                update(arr, heap_index[v->label]); //upward heapify
            }
            v = v->next; // move to adjacent
        }
    }
    int pred_list[SIZE];
    k = 0;
    while ((arr[heap_index[target]].predecessor != -1)
            && (target != source)) {  
        pred_list[k] = target;
        target = arr[heap_index[target]].predecessor
        k++;
    }
    pred_list[k] = source; // add source to end of list
    // list now in reverse order
    for (int l = k; l >= 0; l--) {
        if (l != 0) printf("%d ", pred_list[l]);
        else printf("%d", pred_list[l]);
    }
    printf("\n");

}

void dequeue(int* arr, int n) { // n is the last index
    int temp = arr[n]; // exchange the root and the last node
    arr[n] = arr[0];
    arr[0] = temp;
    n--; // decrease the heap size
    int i = 0, j;
    while ((j = 2*i+1) <= n) { // left child exists?
        if (j < n && arr[j] < arr[j+1]) // if right child greater
        j = j+1; // pick right child
        if (temp >= arr[j]) break;
        else { // move large child
            arr[i] = arr[j];
            i = j;
        }
    }
    arr[i] = temp;
}

void update(struct tnode* arr, int i)
{
    //Don’t forget to update heap_index
    int temp;
    while (i != 0 && arr[i].distance < arr[(i - 1) / 2].distance) {
        // Swap nodes in the heap and update the heap index
        temp = heap_index[arr[i].label]; // swap heap index
        heap_index[arr[i].label] = heap_index[arr[(i - 1) / 2].label];
        heap_index[arr[(i - 1) / 2].label] = temp;

        tnode tmp = arr[i]; // swap tnodes in heap
        arr[i] = arr[(i - 1) / 2];
        arr[(i - 1) / 2] = tmp;

        i = (i - 1) / 2; // update i
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) return 0; // args ./a6, file

    char* in_file = argv[1];
    FILE* file = (FILE*) fopen(in_file, "r"); // open txt file
    assert(file != NULL);

    // V N (# of vertices, period)
    // vs vt w1 . . . wN 
    char line[25];
    int vertices;
    int period;
    fscanf(file, "%d %d", &vertices, &period);
    
    graph = (gnode**) malloc(vertices * sizeof(gnode));
    int vs;
    int vt;
    int weights[period];
    while (fgets(line, sizeof(line), file)) {
        scanf(line, "%d %d", &vs, &vt);    
        for (int i = 0; i < period; i++) {
            scanf(line, "%d", &weights[i]);
            //make graph node w this
            add_edge(vs, vt, weights, period);
        }
    }
    fclose(file);
    
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