#include <stdio.h>

#define V 3
#define INF 99999

void printMatrix(int dist[V][V]) {
    int i, j;
    for(i = 0; i < V; i++) {
        for(j = 0; j < V; j++) {
            if(dist[i][j] == INF)
                printf("INF ");
            else
                printf("%d ", dist[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printPath(int next[V][V], int i, int j) {
    printf("%d", i + 1);
    while(i != j) {
        i = next[i][j];
        printf(" -> %d", i + 1);
    }
}

void floydWarshall(int graph[V][V]) {
    int dist[V][V], next[V][V];
    int i, j, k;

    for(i = 0; i < V; i++) {
        for(j = 0; j < V; j++) {
            dist[i][j] = graph[i][j];
            if(graph[i][j] == INF || i == j)
                next[i][j] = j;
            else
                next[i][j] = j;
        }
    }

    for(k = 0; k < V; k++) {
        for(i = 0; i < V; i++) {
            for(j = 0; j < V; j++) {
                if(dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
        printMatrix(dist);
    }

    printf("Path:\n");
    for(i = 0; i < V; i++) {
        for(j = 0; j < V; j++) {
            if(i != j) {
                printPath(next, i, j);
                printf(" : %d\n", dist[i][j]);
            }
        }
    }
}

int main() {
    int graph[V][V] = {
        {0, 4, 11},
        {6, 0, 2},
        {3, INF, 0}
    };

    floydWarshall(graph);
    return 0;
}
