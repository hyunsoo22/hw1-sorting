/* Iterative max-heap sort. Swapping distant records can break stability. */
#include "sortctx.h"

static void siftDown(SortCtx *c, size_t root, size_t count) {
    while (root < count / 2) {
        size_t child = 2 * root + 1;
        if (child + 1 < count && sortCompareAt(c, child, child + 1) < 0)
            child++;
        if (sortCompareAt(c, root, child) >= 0) break;
        sortSwap(c, root, child);
        root = child;
    }
}

void heapSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats) {
    SortCtx c;
    if (!sortBegin(&c, base, n, size, cmp, stats)) return;
    for (size_t start = n / 2; start > 0; start--) siftDown(&c, start - 1, n);
    for (size_t end = n - 1; end > 0; end--) {
        sortSwap(&c, 0, end);
        siftDown(&c, 0, end);
    }
    sortEnd(&c);
}
