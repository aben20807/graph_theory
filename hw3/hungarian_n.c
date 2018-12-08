#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

int **create_G(const int n);
void print_G(int **g, const int n);
void destroy_G(int **g, const int n);
int **compute_new_w(const int n, int **w, const int *u, const int *v);
int **get_Guv(int **w, const int n);
bool dfs(int **guv, const int i, const int m, bool *vis, int *set_T);
int *min_vertex_cover(int **guv, const int n);
bool is_perfect_matching(const int *set_T, const int n);
int compute_epsilon(int **w, const int n, const int *set_T);
void adjust_u_v(int *u, int *v, const int n, const int *set_T, const int epsilon);

int main()
{
    int n, m;
    scanf("%d", &n);
    m = n;
    int sq_size = (n > m ? n : m);
    int *u = calloc(n, sizeof(int));
    int *min = calloc(n, sizeof(int));
    int *v = calloc(m, sizeof(int));
    int **w = create_G(sq_size);
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

    for (int i = 0; i < sq_size; i++) {
        for (int j = 0; j < sq_size; j++) {
            w[i][j] += (w[i][j] == INT_MIN ? min[i] : 0);
        }
    }

    bool is_perfect = false;
    int *ans_T = calloc(sq_size, sizeof(int));
    for (int i = 0; i < sq_size; i++) {
        ans_T[i] = -1;
    }
    do {
        int **new_w = compute_new_w(sq_size, w, u, v);
        int **Guv = get_Guv(new_w, sq_size);
        int *set_T = min_vertex_cover(Guv, sq_size);

        is_perfect = is_perfect_matching(set_T, sq_size);
        if (is_perfect) {
            for (int i = 0; i < sq_size; i++) {
                ans_T[i] = set_T[i];
            }
        }

        int epsilon = compute_epsilon(new_w, sq_size, set_T);
        adjust_u_v(u, v, sq_size, set_T, epsilon);

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
            ret[i][j] = INT_MIN;
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

int **get_Guv(int **w, const int n)
{
    int **ret = create_G(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
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

int *min_vertex_cover(int **guv, const int n)
{
    int *set_T = calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
        set_T[i] = -1;
    }
    int result = 0;
    for (int i = 0; i < n; i++) {
        bool vis[n];
        for (int j = 0; j < n; j++) {
            vis[j] = false;
        }
        if (dfs(guv, i, n, vis, set_T)) {
            result++;
        }
    }
    return set_T;
}

bool is_perfect_matching(const int *set_T, const int n)
{
    int matching = 0;
    for (int i = 0; i < n; i++) {
        if (set_T[i] != -1) {
            matching++;
        }
    }
    return matching == n;
}

int compute_epsilon(int **w, const int n, const int *set_T)
{
    int epsilon = INT_MAX;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (set_T[j] != -1) {
                continue;
            }
            epsilon = epsilon < w[i][j] ? epsilon : w[i][j];
        }
    }
    return epsilon;
}

void adjust_u_v(int *u, int *v, const int n, const int *set_T, const int epsilon)
{
    for (int i = 0; i < n; i++) {
        u[i] -= epsilon;
    }
    for (int i = 0; i < n; i++) {
        if (set_T[i] != -1) {
            v[i] += epsilon;
        }
    }
}
