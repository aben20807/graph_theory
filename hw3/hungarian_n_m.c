#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

int **create_G(const int n);
void print_G(int **g, const int n);
void destroy_G(int **g, const int n);
int **compute_new_w(const int n, int **w, const int *u, const int *v);
int **get_equality_subgraph(int **w, const int n);
// int **get_adj(int *set_X, const int n);
int **get_adj(int **g, const int n);
bool dfs_bm(int **guv, const int i, const int m, bool *vis, int *set_T);
int *max_bipartite_matching(int **guv, const int n);
// bool dfs_vc(int **g, const int i, const int m, const bool is_X, bool *vis_X, bool *vis_Y);
// bool dfs_vc(int **g, const int i, const int m, const bool is_X, bool *vis_X, bool *vis_Y, int *set_Y);
// void dfs_vc(int **M, const int i, const int n, int *vis);
// void dfs_vc(int **g, const int i, const int n, int *vis, int *set_Y);
void dfs_vc(int **g, const int i, const int n, int *mark_X, int *mark_Y, int *set_Y);
int *min_vertex_cover(int **g, int *set_Y, const int n);
bool is_perfect_matching(const int *set_T, const int n, const int matching_size);
int compute_epsilon(int **w, const int n, const int *min_vc);
void adjust_u_v(int *u, int *v, const int n, const int *min_vc, const int epsilon);

int main()
{
    int n, m;
    scanf("%d", &n);
    scanf("%d", &m);
    int sq_size = (n > m ? n : m);
    int matching_size = (n < m ? n : m) + 1;
    int *u = calloc(sq_size, sizeof(int));
    // XXX min
    int *min = calloc(sq_size, sizeof(int));
    int *v = calloc(sq_size, sizeof(int));
    int **w = create_G(sq_size);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            scanf("%d", &w[i][j]);
            // u_i = max_j w_ij
        }
    }

    for (int i = 0; i < sq_size; i++) {
        for (int j = 0; j < sq_size; j++) {
            if (!j) {
                u[i] = w[i][j];
                min[i] = w[i][j];
            } else {
                u[i] = u[i] > w[i][j] ? u[i] : w[i][j];
                min[i] = min[i] < w[i][j] ? min[i] : w[i][j];
            }
            // w[i][j] += (w[i][j] == INT_MIN ? min[i] : 0);
            // w[i][j] = (w[i][j] == INT_MIN ? 0 : w[i][j]);
        }
    }
    // print_G(w, sq_size);

    bool is_perfect = false;
    int *ans_T = calloc(sq_size, sizeof(int));
    for (int i = 0; i < sq_size; i++) {
        ans_T[i] = -1;
    }
    do {
        int **new_w = compute_new_w(sq_size, w, u, v);
        // print_G(new_w, sq_size);
        int **Guv = get_equality_subgraph(new_w, sq_size);
        // print_G(Guv, sq_size);
        int *set_T = max_bipartite_matching(Guv, sq_size);
        // for (int i = 0; i < sq_size; i++) {
        //     printf("%d ", set_T[i]);
        // }
        int *min_vc = min_vertex_cover(Guv, set_T, sq_size);

        is_perfect = is_perfect_matching(set_T, sq_size, matching_size);
        if (is_perfect) {
            for (int i = 0; i < sq_size; i++) {
                ans_T[i] = set_T[i];
            }
        }

        int epsilon = compute_epsilon(new_w, sq_size, min_vc);
        // for (int i = 0; i < sq_size; i++) {
        //     printf("%d ", u[i]);
        // }
        // printf("\n");
        // for (int i = 0; i < sq_size; i++) {
        //     printf("%d ", v[i]);
        // }
        adjust_u_v(u, v, sq_size, min_vc, epsilon);
        // for (int i = 0; i < sq_size; i++) {
        //     printf("%d ", u[i]);
        // }
        // printf("\n");
        // for (int i = 0; i < sq_size; i++) {
        //     printf("%d ", v[i]);
        // }
        // printf("\n");

        free(min_vc);
        free(set_T);
        destroy_G(Guv, sq_size);
        destroy_G(new_w, sq_size);
    } while (!is_perfect);


    int max_w = 0;
    for (int i = 0; i < sq_size; i++) {
        if (ans_T[i] != -1 && w[ans_T[i]][i] >= 0) {
            max_w += w[ans_T[i]][i];
        }
    }
    printf("%d", max_w);

    destroy_G(w, sq_size);
    free(ans_T);
    free(v);
    free(u);
    return 0;
}

int **create_G(const int n)
{
    int **ret = (int **)calloc(n, sizeof(int *));
    for (int i = 0; i < n; i++) {
        ret[i] = calloc(n, sizeof(int));
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // ret[i][j] = INT_MIN;
            ret[i][j] = 0;
        }
    }
    return ret;
}

void print_G(int **g, const int n)
{
    printf("\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
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

int **compute_new_w(const int n, int **w, const int *u, const int *v)
{
    int **ret = create_G(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ret[i][j] = u[i] + v[j] - w[i][j];
        }
    }
    return ret;
}

int **get_equality_subgraph(int **w, const int n)
{
    int **ret = create_G(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ret[i][j] = w[i][j] == 0 ? 1 : 0;
        }
    }
    return ret;
}

int **get_adj(int **g, const int n)
{
    int **ret = create_G(2 * n);
    for (int i = 0; i < 2 * n; i++) {
        for (int j = 0; j < 2 * n; j++) {
            ret[i][j] = 0;
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ret[i][j + n] = (g[i][j] == 1 ? 1 : 0);
            ret[j + n][i] = (g[i][j] == 1 ? 1 : 0);
        }
    }
    return ret;
}

// get_matching
// int **get_adj(int *set_Y, const int n)
// {
//     int **ret = create_G(2 * n);
//     for (int i = 0; i < 2 * n; i++) {
//         for (int j = 0; j < 2 * n; j++) {
//             ret[i][j] = 0;
//         }
//     }
//     for (int i = 0; i < n; i++) {
//         if (set_Y[i] != -1) {
//             ret[i][set_Y[i] + n] = 1;
//         }
//     }
//     return ret;
// }

bool dfs_bm(int **guv, const int i, const int m, bool *vis, int *set_Y)
{
    for (int j = 0; j < m; j++) {
        if (guv[i][j] && !vis[j]) {
            vis[j] = true;
            if (set_Y[j] == -1 || dfs_bm(guv, set_Y[j], m, vis, set_Y)) {
                set_Y[j] = i;
                return true;
            }
        }
    }
    return false;
}

int *max_bipartite_matching(int **guv, const int n)
{
    int *set_Y = calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
        set_Y[i] = -1;
    }
    int result = 0;
    for (int i = 0; i < n; i++) {
        bool vis[n];
        for (int j = 0; j < n; j++) {
            vis[j] = false;
        }
        if (dfs_bm(guv, i, n, vis, set_Y)) {
            result++;
        }
    }
    return set_Y;
}

void dfs_vc(int **g, const int i, const int n, int *mark_X, int *mark_Y, int *set_Y)
{
    if (mark_X[i]) {
       return;
    }
    mark_X[i] = 1;

    for (int j = 0; j < n; j++) {
        if (g[i][j]) {
            mark_Y[j] = 1;
            // printf("(%d, %d)\n", i, j);
            dfs_vc(g, set_Y[j], n, mark_X, mark_Y, set_Y);
        }
    }
}

int *min_vertex_cover(int **g, int *set_Y, const int n)
{
    // [X(R) | Y(T)]
    int *mvc = calloc(n * 2, sizeof(int));
    int *mark_X = calloc(n, sizeof(int));
    int *mark_Y = calloc(n, sizeof(int));
    int set_X[n];
    for (int i = 0; i < n; i++) {
        set_X[i] = -1;
    }
    for (int i = 0; i < n; i++) {
        int x_matched_idx = set_Y[i];
        if (x_matched_idx != -1) {
            set_X[x_matched_idx] = i;
        }
    }
    // int **g_adj = get_adj(g, n);
    // print_G(g_adj, 2 * n);
    for (int i = 0; i < n; i++) {
        if (set_X[i] == -1) {
            dfs_vc(g, i, n, mark_X, mark_Y, set_Y);
        }
    }
    for (int i = 0; i < n; i++) {
        mvc[i] = !mark_X[i];
    }
    for (int i = 0; i < n; i++) {
        mvc[i + n] = mark_Y[i];
    }
    for (int i = 0; i < 2 * n; i++) {
        // printf("%d ", set_X[i]);
        // printf("%d \n", set_Y[i]);
        // printf("%d ", mvc[i]);
    }
    // printf("\n");
    // destroy_G(g_adj, n);
    return mvc;
}

// XXX matching size
bool is_perfect_matching(const int *set_T, const int n, const int matching_size)
{
    int matching = 0;
    for (int i = 0; i < n; i++) {
        if (set_T[i] != -1) {
            matching++;
        }
    }
    return matching == n;
}

int compute_epsilon(int **w, const int n, const int *min_vc)
{
    int epsilon = INT_MAX;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (min_vc[i] || min_vc[j + n]) {
                continue;
            }
            // printf("%d ", w[i][j]);
            epsilon = epsilon < w[i][j] ? epsilon : w[i][j];
        }
        // printf("\n");
    }
    // print_G(w, n);
    // printf("%d\n", epsilon);
    return epsilon;
}

void adjust_u_v(int *u, int *v, const int n, const int *min_vc, const int epsilon)
{
    for (int i = 0; i < n; i++) {
        if (!min_vc[i]) {
            u[i] -= epsilon;
        }
    }
    for (int i = 0; i < n; i++) {
        if (min_vc[i + n]) {
            v[i] += epsilon;
        }
    }
}
