#include <stdio.h>
#include <limits.h>

#define MAX 10

int m[MAX][MAX];
int s[MAX][MAX];

void printOptimalParenthesis(int i, int j)
{
    if(i == j)
    {
        printf("A%d", i);
    }
    else
    {
        printf("(");

        printOptimalParenthesis(i, s[i][j]);

        printOptimalParenthesis(s[i][j] + 1, j);

        printf(")");
    }
}

int matrixChainOrder(int p[], int n)
{
    int i, j, k, L, q;

    for(i = 1; i < n; i++)
    {
        m[i][i] = 0;
    }

    for(L = 2; L < n; L++)
    {
        for(i = 1; i < n - L + 1; i++)
        {
            j = i + L - 1;

            m[i][j] = INT_MAX;

            for(k = i; k <= j - 1; k++)
            {
                q = m[i][k] + m[k + 1][j] + p[i - 1] * p[k] * p[j];

                if(q < m[i][j])
                {
                    m[i][j] = q;
                    s[i][j] = k;
                }
            }
        }
    }

    return m[1][n - 1];
}

int main()
{
    int n, i;

    printf("Enter number of matrices: ");
    scanf("%d", &n);

    int p[n + 1];

    printf("Enter dimensions:\n");

    for(i = 0; i <= n; i++)
    {
        scanf("%d", &p[i]);
    }

    int minCost = matrixChainOrder(p, n + 1);

    printf("\nMinimum number of scalar multiplications = %d\n", minCost);

    printf("Optimal Parenthesization is ");

    printOptimalParenthesis(1, n);

    printf("\n");

    return 0;
}