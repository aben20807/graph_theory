#include <stdio.h>

int main()
{
    int n;
    scanf("%d", &n);
    int w[n][n];
    int u[n];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &w[i][j]);

            // u_i = max_j w_ij
            if (!j) {
                u[i] = w[i][j];
            } else {
                u[i] = u[i] > w[i][j] ? u[i] : w[i][j];
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", w[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", u[i]);
    }
    printf("\n");

    return 0;
}
