#include <stdio.h>

#define V 7
#define E 10
#define INF 99999

struct Edge {
    int src, dest, weight;
};

void printTable(int dist[], int k) {
    int i;
    printf("%d\t", k);
    for(i = 0; i < V; i++) {
        if(dist[i] == INF)
            printf("INF\t");
        else
            printf("%d\t", dist[i]);
    }
    printf("\n");
}

void printPath(int parent[], int j) {
    if(parent[j] == -1) {
        printf("%d", j + 1);
        return;
    }
    printPath(parent, parent[j]);
    printf("->%d", j + 1);
}

void bellmanFord(struct Edge edges[]) {
    int dist[V], parent[V];
    int i, j;

    for(i = 0; i < V; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }

    dist[0] = 0;

    printf("k\t");
    for(i = 0; i < V; i++) printf("1->%d\t", i + 1);
    printf("\n");

    for(i = 1; i <= V - 1; i++) {
        for(j = 0; j < E; j++) {
            int u = edges[j].src;
            int v = edges[j].dest;
            int w = edges[j].weight;

            if(dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
            }
        }
        printTable(dist, i);
    }

    for(j = 0; j < E; j++) {
        int u = edges[j].src;
        int v = edges[j].dest;
        int w = edges[j].weight;
        if(dist[u] != INF && dist[u] + w < dist[v]) {
            printf("Negative cycle detected\n");
            return;
        }
    }

    printf("Path:\n");
    for(i = 1; i < V; i++) {
        printPath(parent, i);
        printf(":%d\n", dist[i]);
    }
}

int main() {
    struct Edge edges[E] = {
        {0,1,6},
        {0,2,5},
        {0,3,5},
        {1,4,-1},
        {2,1,-2},
        {2,4,1},
        {3,2,-2},
        {3,5,-1},
        {4,6,3},
        {5,6,3}
    };

    bellmanFord(edges);
    return 0;
}
