#include "bench.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *keys[] = {"random", "sorted", "reversed", "few-unique"};

static int measure(const char *scope, InputKind kind, size_t n, int csv) {
    Record *input = malloc(n * sizeof(*input));
    if (input == NULL) return 0;
    makeInput(input, n, kind, 20260930u);
    for (size_t k = 0; k < SORT_ALGORITHM_COUNT; k++) {
        BenchResult r = benchRun(&SORT_ALGORITHMS[k], input, n, 11);
        if (!r.sorted) { free(input); return 0; }
        if (csv)
            printf("%s,%s,%zu,%s,%.6f,%zu,%zu,%zu,%zu,%d,%d,%.6f,%.6f,%.6f,11\n",
                   scope, keys[kind], n, r.algo->name, r.millis,
                   r.stats.compares, r.stats.moves, r.stats.extraBytes,
                   r.stats.maxDepth, r.sorted, r.stable,r.meanMillis,r.minMillis,r.maxMillis);
        else
            printf("%-6s %-10s n=%-5zu %-14s %10.6f ms C=%-9zu M=%-9zu B=%-6zu stable=%d\n",
                   scope, keys[kind], n, r.algo->name, r.millis,
                   r.stats.compares, r.stats.moves, r.stats.extraBytes, r.stable);
    }
    free(input);
    return 1;
}

int main(int argc, char **argv) {
    int csv = argc > 1 && strcmp(argv[1], "--csv") == 0;
    if (csv) puts("scope,input,n,algo,millis,compares,moves,extraBytes,maxDepth,sorted,stable,meanMillis,minMillis,maxMillis,reps");
    for (int kind = 0; kind < INPUT_KIND_COUNT; kind++)
        if (!measure("kinds", (InputKind)kind, 4000, csv)) return EXIT_FAILURE;
    const size_t sizes[] = {1000, 2000, 4000, 8000};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++)
        if (!measure("growth", INPUT_RANDOM, sizes[i], csv)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
