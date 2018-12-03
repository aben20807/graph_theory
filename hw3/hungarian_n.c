#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int **create_G(const int n, const int m);
void print_G(int **g, const int n, const int m);
void destroy_G(int **g, const int n);
int **compute_new_w(const int n, const int m, int **w, const int *u, const int *v);
int **get_Guv(int **w, const int n, const int m);
bool dfs(int **guv, const int i, const int m, bool *vis, int *set_T);
int *min_vertex_cover(int **guv, const int n, const int m);
bool is_perfect_matching(const int *set_T, const int m);
int compute_epsilon(int **w, const int n, const int m, const int *set_T);
void adjust_u_v(int *u, int *v, const int n, const int m, const int *set_T, const int epsilon);

int main()
{
    int n, m;
    scanf("%d", &n);
    m = n;
    int *u = calloc(n, sizeof(int));
    int *v = calloc(m, sizeof(int));
    int **w = create_G(n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            scanf("%d", &w[i][j]);

            // u_i = max_j w_ij
            if (!j) {
                u[i] = w[i][j];
            } else {
                u[i] = u[i] > w[i][j] ? u[i] : w[i][j];
            }
        }
    }
    print_G(w, n, m);

    int **new_w = compute_new_w(n, m, w, u, v);
    print_G(new_w, n, m);

    int **Guv = get_Guv(new_w, n, m);
    print_G(Guv, n, m);

    int *set_T = min_vertex_cover(Guv, n, m);
    for (int i = 0; i < m; i++) {
        // printf("%d ", set_T[i]);
    }
    printf("%d\n", is_perfect_matching(set_T, m));
    int epsilon = compute_epsilon(new_w, n, m, set_T);
    for (int i = 0; i < n; i++) {
        printf("%d ", u[i]);
    }
    printf("\n");
    for (int i = 0; i < m; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    adjust_u_v(u, v, n, m, set_T, epsilon);

    for (int i = 0; i < n; i++) {
        printf("%d ", u[i]);
    }
    printf("\n");
    for (int i = 0; i < m; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");


    free(set_T);
    destroy_G(Guv, n);
    destroy_G(new_w, n);
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

int **compute_new_w(const int n, const int m, int **w, const int *u, const int *v)
{
    int **ret = create_G(n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            ret[i][j] = u[i] + v[j] - w[i][j];
        }
    }
    return ret;
}

int **get_Guv(int **w, const int n, const int m)
{
    int **ret = create_G(n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            ret[i][j] = w[i][j] == 0 ? 1 : 0;
        }
    }
    return ret;
}

bool dfs(int **guv, const int i, const int m, bool *vis, int *set_T)
{
    for (int j = 0; j < m; j++) {
        if (guv[i][j] && !vis[j]) {
            vis[j] = true;
            if (set_T[j] == -1 || dfs(guv, set_T[j], m, vis, set_T)) {
                set_T[j] = i;
                return true;
            }
        }
    }
    return false;
}

int *min_vertex_cover(int **guv, const int n, const int m)
{
    int *set_T = calloc(m, sizeof(int));
    for (int i = 0; i < m; i++) {
        set_T[i] = -1;
    }
    int result = 0;
    for (int i = 0; i < n; i++) {
        bool vis[m];
        memset(vis, 0, sizeof(vis));
        if (dfs(guv, i, m, vis, set_T)) {
            result++;
        }
    }
    return set_T;
}

bool is_perfect_matching(const int *set_T, const int m)
{
    int matching = 0;
    for (int i = 0; i < m; i++) {
        if (set_T[i] != -1) {
            matching++;
        }
    }
    return matching == m;
}

int compute_epsilon(int **w, const int n, const int m, const int *set_T)
{
    int epsilon = 2e9;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (set_T[j] != -1) {
                continue;
            }
            epsilon = epsilon < w[i][j] ? epsilon : w[i][j];
        }
    }
    return epsilon;
}

void adjust_u_v(int *u, int *v, const int n, const int m, const int *set_T, const int epsilon)
{
    for (int i = 0; i < n; i++) {
        u[i] -= epsilon;
    }
    for (int i = 0; i < m; i++) {
        if (set_T[i] != -1) {
            v[i] += epsilon;
        }
    }
}
