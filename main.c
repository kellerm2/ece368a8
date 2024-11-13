#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define SIZE 100

typedef struct gnode {
    int label;
    int weight*;
    struct gnode* next;
}

struct gnode* graph[SIZE];


void create_gnode() {

}

void add_edge() {
    
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
    
    int vs;
    int vt;
    int weights[period];
    while (fgets(line, sizeof(line), file)) {
        scanf(line, "%d %d", &vs, &vt);    
        for (int i = 0; i < period; i++) {
            scanf(line, "%d", &weights[i]);
            //make graph node w this
        }
    }

    while (scanf("%d %d", &user_start, &user_target)) {
        // find shortest path from user_start to user_target
        // print path
    }

    return 0;
}