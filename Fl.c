#include <stdio.h>

#define MAX 10
#define INF 99999

void printMatrix(int dist[MAX][MAX], int n) {
    int i, j;
    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            if(dist[i][j] == INF)
                printf("INF ");
            else
                printf("%d ", dist[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printPath(int next[MAX][MAX], int i, int j) {
    if(next[i][j] == -1)
        return;
    printf("%d ", i + 1);
    while(i != j) {
        i = next[i][j];
        printf("%d ", i + 1);
    }
}

void floydWarshall(int graph[MAX][MAX], int n) {
    int dist[MAX][MAX], next[MAX][MAX];
    int i, j, k;

    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            dist[i][j] = graph[i][j];
            if(graph[i][j] == INF)
                next[i][j] = -1;
            else
                next[i][j] = j;
        }
    }

    for(k = 0; k < n; k++) {
        for(i = 0; i < n; i++) {
            for(j = 0; j < n; j++) {
                if(dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
        printMatrix(dist, n);
    }

    printf("Path:\n");
    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            if(i != j) {
                printf("%d -> %d: ", i + 1, j + 1);
                printPath(next, i, j);
                printf("\n");
            }
        }
    }
}

int main() {
    int n, i, j;
    int graph[MAX][MAX];

    printf("Enter number of vertices: ");
    scanf("%d", &n);

    printf("Enter adjacency matrix (enter %d for no edge):\n", INF);
    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            scanf("%d", &graph[i][j]);
        }
    }

    floydWarshall(graph, n);
    return 0;
}
