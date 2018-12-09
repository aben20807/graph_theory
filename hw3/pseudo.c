int main()
{
    scanf(n);
    scanf(m);
    /* Expand the n by m matrix to a square,
       choose the bigger one as the size */
    int sq_size = (n > m ? n : m);
    i64 **w = sq_size by sqsize 2D array;
    /* Input weight map */
    scanf(w);

    /* Store the max value in each row into u array */
    i64 u[sq_size] = max value foreach row in w;
    i64 v[sq_size] = all 0;
    bool is_perfect = false;

    /* Hungarian algorithm */
    do {
        i64 **new_w = compute_new_w(sq_size, w, u, v);
        i64 **guv = get_equality_subgraph(new_w, sq_size);
        int *set_T = max_bipartite_matching(guv, sq_size);
        int *min_vc = min_vertex_cover(guv, set_T, sq_size);

        is_perfect = is_perfect_matching(set_T, sq_size);
        if (is_perfect) {
            ans_T = set_T;
            break;
        }

        i64 epsilon = compute_epsilon(new_w, sq_size, min_vc);
        adjust_u_v(u, v, sq_size, min_vc, epsilon);
    } while (!is_perfect);

    i64 max_w = 0;
    max_w += all corresponding w from ans_T;
    printf(max_w);
}
