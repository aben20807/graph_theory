#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

typedef long long i64;

i64 **create_G(const int n);
void print_G(i64 **g, const int n);
void destroy_G(i64 **g, const int n);
i64 **compute_new_w(const int n, i64 **w, const i64 *u, const i64 *v);
i64 **get_equality_subgraph(i64 **w, const int n);
int **get_adj(i64 **g, const int n);
bool dfs_bm(i64 **guv, const int i, const int m, bool *vis, int *set_T);
int *max_bipartite_matching(i64 **guv, const int n);
void dfs_vc(i64 **g, const int i, const int n, int *mark_X, int *mark_Y, int *set_Y);
int *min_vertex_cover(i64 **g, int *set_Y, const int n);
bool is_perfect_matching(const int *set_T, const int n);
i64 compute_epsilon(i64 **w, const int n, const int *min_vc);
void adjust_u_v(i64 *u, i64 *v, const int n, const int *min_vc, const i64 epsilon);

void set_array_i32(int *a, const int n, const int value);
void print_array_i32(const int *a, const int n);
void init_array_i64(i64 *a, const int n, const i64 value);
void print_array_i64(const i64 *a, const int n);

int main()
{
    int n, m;
    scanf("%d", &n);
    scanf("%d", &m);
    /* Expand the n by m matrix to a square,
       choose the bigger one as the size */
    int sq_size = (n > m ? n : m);
    i64 *u = calloc(sq_size, sizeof(i64));
    i64 *v = calloc(sq_size, sizeof(i64));
    i64 **w = create_G(sq_size);
    /* Input weight map */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            scanf("%lld", &w[i][j]);
        }
    }
    // DEBUG: print the weight map
    // print_G(w, sq_size);

    /* Store the max value in each row into u array */
    for (int i = 0; i < sq_size; i++) {
        for (int j = 0; j < sq_size; j++) {
            if (!j) { // let u initialized to the first element
                u[i] = w[i][j];
            } else {
                u[i] = u[i] > w[i][j] ? u[i] : w[i][j];
            }
        }
    }

    bool is_perfect = false;
    int *ans_T = calloc(sq_size, sizeof(int));
    set_array_i32(ans_T, sq_size, -1);

    /* Hungarian algorithm */
    do {
        i64 **new_w = compute_new_w(sq_size, w, u, v);
        // print_G(new_w, sq_size);
        i64 **guv = get_equality_subgraph(new_w, sq_size);
        // print_G(guv, sq_size);
        int *set_T = max_bipartite_matching(guv, sq_size);
        // print_array_i32(set_T, sq_size);

        int *min_vc = min_vertex_cover(guv, set_T, sq_size);

        is_perfect = is_perfect_matching(set_T, sq_size);
        if (is_perfect) {
            for (int i = 0; i < sq_size; i++) {
                ans_T[i] = set_T[i];
            }
        }

        i64 epsilon = compute_epsilon(new_w, sq_size, min_vc);
        // printf("%lld\n", epsilon);
        adjust_u_v(u, v, sq_size, min_vc, epsilon);
        // DEBUG: print the content of u and v
        // print_array_i64(u, sq_size);
        // print_array_i64(v, sq_size);

        free(min_vc);
        free(set_T);
        destroy_G(guv, sq_size);
        destroy_G(new_w, sq_size);
    } while (!is_perfect);

    i64 max_w = 0;
    for (int i = 0; i < sq_size; i++) {
        max_w += w[ans_T[i]][i];
    }
    printf("%lld", max_w);

    destroy_G(w, sq_size);
    free(ans_T);
    free(v);
    free(u);
    return 0;
}

/*
 * Function: create_G
 *  Malloc a n by n i64 type matrix (2D array) and initialize all elements
 *  to 0 (by calloc).
 *
 *  n: the number of max size between X and Y size.
 *
 *  Returns: a empty graph matrix.
 */
i64 **create_G(const int n)
{
    i64 **ret = (i64 **)calloc(n, sizeof(i64 *));
    for (int i = 0; i < n; i++) {
        ret[i] = calloc(n, sizeof(i64));
    }
    return ret;
}

void print_G(i64 **g, const int n)
{
    printf("\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%lld ", g[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void destroy_G(i64 **g, const int n)
{
    for (int i = 0; i < n; i++) {
        free(g[i]);
    }
    free(g);
}

/*
 * Function: compute_new_w
 *  compute the new weight in Hungarian algorithm.
 */
i64 **compute_new_w(const int n, i64 **w, const i64 *u, const i64 *v)
{
    i64 **ret = create_G(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ret[i][j] = u[i] + v[j] - w[i][j];
        }
    }
    return ret;
}

/*
 * Function: get_equality_subgraph
 *  Use weight map to create the equality subgraph guv.
 *  If the weight is 0 then let there connected in guv.
 *
 *  w: the weight map that from the result of compute_new_w().
 *  n: size.
 *
 *  Returns: the equality subgraph guv.
 */
i64 **get_equality_subgraph(i64 **w, const int n)
{
    i64 **ret = create_G(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ret[i][j] = w[i][j] == 0 ? 1 : 0;
        }
    }
    return ret;
}

bool dfs_bm(i64 **guv, const int i, const int m, bool *vis, int *set_Y)
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

/*
 * Function: max_bipartite_matching
 *  Find the maximum bipartite matching of guv and return the matching
 *  situation of set_Y.
 *
 *  Returns: set_Y
 *      the element value is -1 mean no matching to X set, otherwise the value
 *      is the X set position that the set_Y[i] matched.
 */
int *max_bipartite_matching(i64 **guv, const int n)
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

void dfs_vc(i64 **g, const int i, const int n, int *mark_X, int *mark_Y, int *set_Y)
{
    if (mark_X[i]) {
       return;
    }
    mark_X[i] = 1;

    for (int j = 0; j < n; j++) {
        if (g[i][j]) {
            mark_Y[j] = 1;
            // DEBUG: print out the point visited when compute min vertex cover.
            // printf("(%d, %d)\n", i, j);
            dfs_vc(g, set_Y[j], n, mark_X, mark_Y, set_Y);
        }
    }
}

/*
 * Function: min_vertex_cover
 *  Find a minimum vertex cover of graph g and a matching (knowing by set_Y).
 *
 *  g: input graph (equality subgraph).
 *  set_Y: the matching situation in Y set of one of matchings.
 *  n: the set size.
 *
 *  Returns: the minimum vertex cover (include X, Y set).
 *      the size is 2n, first half is R set (min vc of X), latter half is T
 *      set (min vc of Y).
 */
int *min_vertex_cover(i64 **g, int *set_Y, const int n)
{
    // [R(X) | T(Y)]
    int *mvc = calloc(n * 2, sizeof(int));
    // mark: used to mark point if it will be visited in the algorithm.
    int mark_X[n];
    int mark_Y[n];
    int set_X[n];
    for (int i = 0; i < n; i++) {
        set_X[i] = -1;
        mark_X[i] = 0;
        mark_Y[i] = 0;
    }
    /* Record the X set matching which element of Y set */
    for (int i = 0; i < n; i++) {
        int x_matched_idx = set_Y[i];
        if (x_matched_idx != -1) {
            set_X[x_matched_idx] = i;
        }
    }
    // DEBUG: print the content of set_X and set_Y
    // print_array_i32(set_X, n);
    // print_array_i32(set_Y, n);

    /* Start from the element of X that unsaturated by matching. Need to
       follow the alternating path */
    for (int i = 0; i < n; i++) {
        if (set_X[i] == -1) {
            dfs_vc(g, i, n, mark_X, mark_Y, set_Y);
        }
    }
    /* Copy the mark result to mvc array, choose unmarked elements in X
        and marked elements in Y */
    for (int i = 0; i < n; i++) {
        mvc[i] = !mark_X[i];
        mvc[i + n] = mark_Y[i];
    }

    // DEBUG: print the content of mvc
    // print_array_i32(mvc, 2 * n);
    return mvc;
}

/*
 * Function: is_perfect_matching
 *  If the set_Y are all matched with X (all values of elements are not -1)
 *  then mean there is a perfect matching.
 */
bool is_perfect_matching(const int *set_Y, const int n)
{
    int matching = 0;
    for (int i = 0; i < n; i++) {
        if (set_Y[i] != -1) {
            matching++;
        }
    }
    return matching == n;
}

/*
 * Function: compute_epsilon
 *  Ignore the column and row of minimum vertex cover and find the minimum
 *  value as the epsilon.
 *
 *  w: the original weight map.
 *  n: size.
 *  min_vc: the minimum vertex cover of equality subgraph.
 */
i64 compute_epsilon(i64 **w, const int n, const int *min_vc)
{
    int epsilon = INT_MAX;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (min_vc[i] || min_vc[j + n]) {
                continue;
            }
            epsilon = epsilon < w[i][j] ? epsilon : w[i][j];
        }
    }
    return epsilon;
}

/*
 * Function: adjust_u_v
 *  Used to reconstruct u and v by epsilon.
 *  Decrease u_i by epsilon for x_i belong X - R, and increase v_i by epsilon
 *  for y_i belong T. R and T are infered from min_vc array.
 *
 *  u: the u set needed to be adjusted.
 *  v: the v set needed to be adjusted.
 *  n: size.
 *  min_vc: the minimum vertex cover of equality subgraph.
 *  epsilon: the adjustment value.
 */
void adjust_u_v(i64 *u, i64 *v, const int n, const int *min_vc, const i64 epsilon)
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

void set_array_i32(int *a, const int n, const int value)
{
    for (int i = 0; i < n; i++) {
        a[i] = value;
    }
}

void print_array_i32(const int *a, const int n)
{
    for (int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

void init_array_i64(i64 *a, const int n, const i64 value)
{
    for (int i = 0; i < n; i++) {
        a[i] = value;
    }
}

void print_array_i64(const i64 *a, const int n)
{
    for (int i = 0; i < n; i++) {
        printf("%lld ", a[i]);
    }
    printf("\n");
}
