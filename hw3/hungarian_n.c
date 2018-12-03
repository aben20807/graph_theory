#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

int **create_G(const int n, const int m);
void print_G(int **g, const int n, const int m);
void destroy_G(int **g, const int n);
int **compute_new_w(const int n, const int m, int **w, const int *u, const int *v);
int **get_Guv(int **w, const int n, const int m);
bool dfs(int **guv, const int i, const int m, bool *vis, int *set_T);
int *min_vertex_cover(int **guv, const int n, const int m);
bool is_perfect_matching(const int *set_T, const int n, const int m);
int compute_epsilon(int **w, const int n, const int m, const int *set_T);
void adjust_u_v(int *u, int *v, const int n, const int m, const int *set_T, const int epsilon);

int main()
{
    int n, m;
    scanf("%d", &n);
    scanf("%d", &m);
    // m = n;
    int sq_size = (n > m ? n : m);
    int *u = calloc(n, sizeof(int));
    int *min = calloc(n, sizeof(int));
    int *v = calloc(m, sizeof(int));
    int **w = create_G(sq_size, sq_size);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            scanf("%d", &w[i][j]);
            // u_i = max_j w_ij
            if (!j) {
                u[i] = w[i][j];
                min[i] = w[i][j];
            } else {
                u[i] = u[i] > w[i][j] ? u[i] : w[i][j];
                min[i] = min[i] < w[i][j] ? min[i] : w[i][j];
            }
        }
    }
    n = sq_size;
    m = sq_size;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            w[i][j] += (w[i][j] == INT_MIN ? min[i] : 0);
        }
    }
    // print_G(w, n, m);

    bool is_perfect = false;
    int *ans_T = calloc(m, sizeof(int));
    for (int i = 0; i < m; i++) {
        ans_T[i] = -1;
    }
    do {
        int **new_w = compute_new_w(n, m, w, u, v);
        // print_G(new_w, n, m);

        int **Guv = get_Guv(new_w, n, m);

        int *set_T = min_vertex_cover(Guv, n, m);
        // for (int i = 0; i < m; i++) {
        //     printf("%d ", set_T[i]);
        // }
        // printf("\n");

        is_perfect = is_perfect_matching(set_T, n, m);
        if (is_perfect) {
            for (int i = 0; i < m; i++) {
                ans_T[i] = set_T[i];
            }
        }

        int epsilon = compute_epsilon(new_w, n, m, set_T);
        adjust_u_v(u, v, n, m, set_T, epsilon);

        free(set_T);
        destroy_G(Guv, n);
        destroy_G(new_w, n);
    } while (!is_perfect);


    int max_w = 0;
    for (int i = 0; i < m; i++) {
        // printf("%d ", ans_T[i]);
        if (ans_T[i] != -1 && w[ans_T[i]][i] >= 0) {
            max_w += w[ans_T[i]][i];
        }
    }
    printf("%d", max_w);

    destroy_G(w, n);
    free(ans_T);
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
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            ret[i][j] = INT_MIN;
        }
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

bool is_perfect_matching(const int *set_T, const int n, const int m)
{
    int matching = 0;
    for (int i = 0; i < m; i++) {
        if (set_T[i] != -1) {
            matching++;
        }
    }
    // return matching == ((m > n) ? n : m);
    return matching == m;
}

int compute_epsilon(int **w, const int n, const int m, const int *set_T)
{
    int epsilon = INT_MAX;
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
