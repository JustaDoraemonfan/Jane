#include <stdio.h>

#define MAX 10

int adj[MAX][MAX], visited[MAX], n;

void bfs(int start) {
    int queue[MAX], front = 0, rear = 0, i;
    for(i = 0; i < n; i++) visited[i] = 0;

    queue[rear++] = start;
    visited[start] = 1;

    while(front < rear) {
        int node = queue[front++];
        printf("%d ", node + 1);

        for(i = 0; i < n; i++) {
            if(adj[node][i] && !visited[i]) {
                queue[rear++] = i;
                visited[i] = 1;
            }
        }
    }
    printf("\n");
}

void dfs(int start) {
    int i;
    visited[start] = 1;
    printf("%d ", start + 1);

    for(i = 0; i < n; i++) {
        if(adj[start][i] && !visited[i]) {
            dfs(i);
        }
    }
}

int main() {
    int i, j, choice, start;

    printf("Enter number of vertices: ");
    scanf("%d", &n);

    printf("Enter adjacency matrix:\n");
    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            scanf("%d", &adj[i][j]);
        }
    }

    while(1) {
        printf("\n1. BFS\n2. DFS\n3. Exit\nEnter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                printf("Enter starting vertex: ");
                scanf("%d", &start);
                bfs(start - 1);
                break;

            case 2:
                for(i = 0; i < n; i++) visited[i] = 0;
                printf("Enter starting vertex: ");
                scanf("%d", &start);
                dfs(start - 1);
                printf("\n");
                break;

            case 3:
                return 0;

            default:
                printf("Invalid choice\n");
        }
    }
}
