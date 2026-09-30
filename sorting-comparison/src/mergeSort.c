/* Stable bottom-up merge sort. No boundary-skip optimization or recursion. */
#include "sortctx.h"
#include <stdio.h>
#include <stdlib.h>

void mergeSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats) {
    SortCtx c;
    if (!sortBegin(&c, base, n, size, cmp, stats)) return;
    char *aux = malloc(n * size);
    if (aux == NULL) {
        fputs("Allocation failed\n", stderr);
        exit(EXIT_FAILURE);
    }
    if (stats != NULL) stats->extraBytes = n * size + size;
    for (size_t width = 1; width < n;) {
        size_t lo = 0;
        while (lo < n) {
            size_t mid = lo + (width < n - lo ? width : n - lo);
            size_t hi = mid + (width < n - mid ? width : n - mid);
            if (mid < hi) {
                size_t i = lo, j = mid, k = lo;
                while (i < mid && j < hi) {
                    /* Take the left record on ties to preserve stability. */
                    size_t chosen = sortCompareAt(&c, i, j) <= 0 ? i++ : j++;
                    sortMove(&c, aux + k++ * size, sortElemAt(&c, chosen));
                }
                while (i < mid) sortMove(&c, aux + k++ * size, sortElemAt(&c, i++));
                while (j < hi) sortMove(&c, aux + k++ * size, sortElemAt(&c, j++));
                for (k = lo; k < hi; k++)
                    sortMove(&c, sortElemAt(&c, k), aux + k * size);
            }
            lo = hi;
        }
        if (width > n / 2) break; /* prevent overflow on the next doubling */
        width *= 2;
    }
    free(aux);
    sortEnd(&c);
}
