#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

typedef struct gnode {
    int label;
    int time;       // Time state
    int weight;     // Weight for this transition
    struct gnode* next;
} gnode;

typedef struct tnode {
    int label;
    int time;
    int distance;
    int predecessor;
    //int steps;
} tnode;

gnode** graph;
int* heap_index;
int* dist_list;
int* t_list;
int* path;

void add_edge(int from, int to, int weight_main[], int period);
void dijkstra(int source, int SIZE, int period, int target);
void dequeue(tnode* arr, int n, int period);
void update(tnode* arr, int i, int period, int n);

void add_edge(int from, int to, int weight_main[], int period) {
    for (int t = 0; t < period; t++) {
        gnode* new = (struct gnode*) malloc(sizeof(struct gnode));
        new->label = to;
        new->time = (t + 1) % period;
        new->weight = weight_main[t];
        new->next = graph[from * period + t];
        graph[from * period + t] = new;
    }
}

// Finds the shortest paths between a source node and all other nodes,
// then prints the traversal of the shortest path from the source node to a target node
// Taken from course slides
void dijkstra(int source, int SIZE, int period, int target) {
    int n = SIZE * period; // n nodes
    struct tnode* arr = (struct tnode*) malloc(n * sizeof(struct tnode));
    heap_index = (int*) malloc(n * sizeof(int));

    for(int i = 0; i < n; i++) { // initialize heap and heap_index
        arr[i].label = i / period;
        arr[i].time = i % period;
        arr[i].distance = INT_MAX;
        arr[i].predecessor = -1;
        //arr[i].steps = 0;
        heap_index[i] = i;
    }
    int source_index = source * period + 0;  // Start at time 0
    arr[source_index].distance = 0;
    
    // swap source vertex to front of heap
    tnode temp = arr[0];
    arr[0] = arr[source_index];
    arr[source_index] = temp;
    heap_index[arr[0].label * period + arr[0].time] = 0;
    heap_index[arr[source_index].label * period + arr[source_index].time] = source_index;
    
    while (n != 0) {
        dequeue(arr, n - 1, period); // downward heapify for sorting δ(s, u), node w/ smallest distance
        n--;
        int u = arr[n].label; // u is current vertex label
        int t = arr[n].time; // t is current time
        struct gnode* v = graph[u * period + t]; // v is adjacency list of u

        while (v != NULL) {
            // step weight
            //int weight_index = arr[heap_index[u]].steps % period;
            //int weight = v->weights[weight_index];
            //printf("The weight from %d to %d is: %d\n", u, v->label, weight);

            int v_index = v->label * period + v->time;
            if (heap_index[v_index] < n && // remainings in heap
            arr[heap_index[v_index]].distance > arr[heap_index[u * period + t]].distance + v->weight) {
                arr[heap_index[v_index]].distance = arr[heap_index[u * period + t]].distance + v->weight; // update distance w/ step weight
                arr[heap_index[v_index]].predecessor = u;
                // printf("UPDATED predecessor: Node %d has pred %d at time %d\n", arr[heap_index[v_index]].label, u, t);
                //arr[heap_index[v->label]].steps = arr[heap_index[u]].steps + 1;
                update(arr, heap_index[v_index], period, n); //upward heapify
            }
            v = v->next; // move to adjacent
        }
    }

    // int dist_list[100];
    // int t_list[100];
    // int path[100];
    dist_list = (int*) malloc(1000000 * sizeof(int));
    t_list = (int*) malloc(1000000 * sizeof(int));
    path = (int*) malloc(1000000 * sizeof(int));
    for (int y=0;y<100;y++) {
        dist_list[y] = 0;
        t_list[y] = 0;
        path[y] = -1;
    }
    int l = 0;
    for (int t = 0; t < period * SIZE; t++) {
        if (arr[t].label == target && arr[t].distance > 0) {
            dist_list[l] = arr[t].distance;
            t_list[l] = t;
            l++;
        }
    }

    int min = dist_list[0];
    int mint = t_list[0];
    for (int m = 1; m < 99; m++) {
        if (dist_list[m] < min && dist_list[m] > 0) {
            min = dist_list[m];
            mint = t_list[m];
        }
        
    }

    // for (int p = 0; p < SIZE * period; p++) {
    //     printf("node: %d i: %d pred: %d\n", arr[heap_index[p]].label, heap_index[p], arr[heap_index[p]].predecessor);
    // }
    //printf("min distance: %d\n", min);
    //printf("%d", mint);
    //printf("%d\n", arr[mint].label);
    int path_index = 0;
    while (arr[mint].predecessor != source) {
        //printf(" %d pred: %d\n", arr[mint].label, arr[mint].predecessor);
        path[path_index] = arr[mint].label;
        // printf("path: %d\n", path[path_index]);
        mint++;
        path_index++;
        // printf("index: %d %d\n", path_index, mint);
    }
    //printf(" %d pred: %d", arr[mint].label, arr[mint].predecessor);
    path[path_index] = arr[mint].label;
    // printf("index: %d %d\n", path_index, mint);
    path_index++;
    path[path_index] = arr[mint].predecessor;
    //printf("FINAL: %d ", arr[mint].predecessor);
    // printf("index: %d %d\n", path_index, mint);

    for (int pi = path_index; pi >= 0; pi--) {
        printf("%d ", path[pi]);
    }
    printf("\n");

    free(dist_list);
    free(t_list);
    free(path);
    free(arr);
    free(heap_index);
}

// dequeues the last node in the min heap, so it is no longer included in the dijkstra
// search for the shortest path, taken from course slides
void dequeue(tnode* arr, int n, int period) { // n is the last index
    tnode temp = arr[n]; // exchange the root and the last node
    arr[n] = arr[0];
    arr[0] = temp;
    // swap the nodes in heap_index
    heap_index[arr[0].label * period + arr[0].time] = 0;
    heap_index[arr[n].label * period + arr[n].time] = n;
    n--; // decrease the heap size
    int i = 0, j;
    while ((j = 2*i+1) <= n) { // while left child exists
        if (j < n && arr[j].distance > arr[j+1].distance) // if right child smaller
        j = j+1; // pick right child
        if (temp.distance <= arr[j].distance) break; // if parent smaller than child, done
        else { // move small child
            arr[i] = arr[j];
            heap_index[arr[i].label * period + arr[i].time] = i; // Update heap_index for the moved node
            i = j;
        }
    }
    arr[i] = temp;
    heap_index[temp.label * period + temp.time] = i; // Update heap_index for the final position
    // for (int k = 0; k <= n; k++) {
    //     if (heap_index[k] != -1) {
    //         printf("Node (%d, %d): Distance = %d\n", arr[k].label, arr[k].time, arr[k].distance);
    //     }
    // }
    // printf("DEQUEUED_______________\n");
}

// Upward heapify the min heap by swapping a node with its parent if it is smaller by the
// definition of a min heap
void update(struct tnode* arr, int i, int period, int n)
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
        heap_index[arr[i].label * period + arr[i].time] = i;
        heap_index[arr[parent].label * period + arr[parent].time] = parent;

        i = parent;
    }
    // printf("Heap after update at index %d:\n", i);
    // for (int j = 0; j <= n; j++) {
    //     if (heap_index[j] != -1) {
    //         printf("Node (%d, %d): Distance = %d\n", arr[j].label, arr[j].time, arr[j].distance);
    //     }
    // }
    // printf("UPDATED_______________\n");
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
    
    graph = (gnode**) malloc(vertices * period * sizeof(gnode*));
    assert(graph != NULL);
    for (int g = 0; g < vertices * period; g++) {
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

    // for (int i = 0; i < vertices; i++) {
    //     for (int t = 0; t < period; t++) { 
    //         int index = i * period + t;
    //         printf("Node (%d, %d):\n", i, t);
            
    //         gnode* current = graph[index];
    //         while (current != NULL) {
    //             printf("  -> (%d, %d) [weight: %d]\n", current->label, current->time, current->weight);
    //             current = current->next;
    //         }
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
    for (int j = 0; j < vertices * period; j++) {
        gnode* current = graph[j];
        while (current != NULL) {
            gnode* tofree = current;
            current = current->next;
            //free(temp->weights);
            free(tofree);
        }
    }
    free(graph);

    return 0;
}
