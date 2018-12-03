#include <stdio.h>
#include <stdlib.h>

int **create_G(const int n, const int m);
void print_G(int **g, const int n, const int m);
void destroy_G(int **g, const int n);
int **compute_Guv(const int n, const int m, int **w, const int *u, const int *v);

int main()
{
    int n, m;
    scanf("%d", &n);
    m = n;
    int *u = calloc(n, sizeof(int));
    int *v = calloc(m, sizeof(int));
    int **w = create_G(n, n);
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
    int **Guv = compute_Guv(n, m, w, u, v);
    print_G(Guv, n, n);

    destroy_G(Guv, n);
    destroy_G(w, n);
    free(v);
    free(u);
    return 0;
}

int **create_G(const int n, const int m)
{
    int **ret = (int **)calloc(n, sizeof(int *));
    for (int i = 0; i < n; i++) {
        ret[i] = calloc(m, sizeof(int));
    }
    return ret;
}

void print_G(int **g, const int n, const int m)
{
    printf("\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%d ", g[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void destroy_G(int **g, const int n)
{
    for (int i = 0; i < n; i++) {
        free(g[i]);
    }
    free(g);
}

int **compute_Guv(const int n, const int m, int **w, const int *u, const int *v)
{
    int **ret = create_G(n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            ret[i][j] = u[i] + v[j] - w[i][j];
        }
    }
    return ret;
}
