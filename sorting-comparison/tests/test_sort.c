#include "bench.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int checks, failures;
static void check(const char *algorithm, const char *name, int pass) {
    checks++;
    failures += !pass;
    printf("%s %-14s %s\n", pass ? "PASS" : "FAIL", algorithm, name);
}

/* Check both sorted keys and preservation of every tagged input record. */
static int validRecords(const Record *a, const Record *input, size_t n, int stable) {
    unsigned char seen[512] = {0};
    if (n > sizeof(seen)) return 0;
    if (!recordsSorted(a, n) || (stable && !recordsStable(a, n))) return 0;
    for (size_t i = 0; i < n; i++) {
        int tag = a[i].tag;
        if (tag < 0 || (size_t)tag >= n || seen[tag]) return 0;
        if (a[i].key != input[tag].key) return 0;
        seen[tag] = 1;
    }
    return 1;
}

static void basicCase(const SortAlgorithm *algo, const char *name, const int *input, size_t n) {
    int a[32] = {0}, expected[32] = {0};
    SortStats stats;
    if (n) { memcpy(a, input, n * sizeof(int)); memcpy(expected, input, n * sizeof(int)); }
    qsort(expected, n, sizeof(int), sortCompareInt);
    algo->sort(a, n, sizeof(int), sortCompareInt, &stats);
    check(algo->name, name, memcmp(a, expected, n * sizeof(int)) == 0);
}

static void varyingSizes(const SortAlgorithm *algo) {
    Record a[512], input[512];
    int pass = 1;
    srand(20260930u);
    for (size_t n = 0; n <= 512 && pass; n++) {
        for (size_t i = 0; i < n; i++) { input[i].key = rand() % 17 - 8; input[i].tag = (int)i; }
        memcpy(a, input, n * sizeof(Record));
        algo->sort(a, n, sizeof(Record), recordCompare, NULL);
        pass = validRecords(a, input, n, algo->stable);
    }
    check(algo->name, "sizes 0..512, tagged permutation and stable guarantees", pass);
}

/* Exhaust every ternary-key input of lengths 0..7 (3,280 arrays). */
static void exhaustive(const SortAlgorithm *algo) {
    Record a[7], input[7];
    int pass = 1;
    size_t total = 1;
    for (size_t n = 0; n <= 7 && pass; n++) {
        for (size_t code = 0; code < total && pass; code++) {
            size_t digits = code;
            for (size_t i = 0; i < n; i++) {
                input[i].key = (int)(digits % 3) - 1; input[i].tag = (int)i; digits /= 3;
            }
            memcpy(a, input, n * sizeof(Record));
            algo->sort(a, n, sizeof(Record), recordCompare, NULL);
            pass = validRecords(a, input, n, algo->stable);
        }
        total *= 3;
    }
    check(algo->name, "exhaustive 3,280 short arrays", pass);
}

int main(void) {
    const int mixed[] = {5,-1,4,2,0,-3};
    const int sorted[] = {-3,-1,0,2,4,5};
    const int reversed[] = {9,7,5,3,1};
    const int duplicates[] = {2,1,2,0,1,0};
    const int equal[] = {4,4,4,4,4};
    const int single[] = {3};
    const int extremes[] = {INT_MAX,0,INT_MIN,INT_MAX,INT_MIN};
    for (size_t k = 0; k < SORT_ALGORITHM_COUNT; k++) {
        const SortAlgorithm *a = &SORT_ALGORITHMS[k];
        basicCase(a,"mixed and negative",mixed,6);
        basicCase(a,"sorted",sorted,6);
        basicCase(a,"reversed",reversed,5);
        basicCase(a,"duplicate keys",duplicates,6);
        basicCase(a,"all equal",equal,5);
        basicCase(a,"one record",single,1);
        basicCase(a,"empty array",NULL,0);
        basicCase(a,"integer extremes",extremes,5);
        varyingSizes(a);
        exhaustive(a);
        Record input[3] = {{2,0},{2,1},{1,2}}, output[3];
        memcpy(output,input,sizeof(input));
        SortStats stats;
        a->sort(output,3,sizeof(Record),recordCompare,&stats);
        check(a->name,"duplicate-key witness", validRecords(output,input,3,0) && recordsStable(output,3)==a->stable);
        printf("WITNESS %s: (%d,%d) (%d,%d) (%d,%d)\n",a->name,
               output[0].key,output[0].tag,output[1].key,output[1].tag,output[2].key,output[2].tag);
        size_t bytes = strcmp(a->name,"mergeSort")==0 ? 4*sizeof(Record) : sizeof(Record);
        check(a->name,"counters and heap allocation",stats.compares>0 && stats.moves>0 && stats.extraBytes==bytes && stats.maxDepth==1);
        Record benchInput[300];
        makeInput(benchInput,300,INPUT_FEW_UNIQUE,20260930u);
        BenchResult b=benchRun(a,benchInput,300,2);
        check(a->name,"benchmark correctness",b.sorted && (a->stable ? b.stable : 1) && b.millis>=0);
    }
    Record good[]={{1,0},{1,1},{2,2}}, badStable[]={{1,1},{1,0},{2,2}}, badSort[]={{2,0},{1,1},{3,2}};
    check("detector","stable sample",recordsSorted(good,3)&&recordsStable(good,3));
    check("detector","stability violation",recordsSorted(badStable,3)&&!recordsStable(badStable,3));
    check("detector","unsorted sample",!recordsSorted(badSort,3));
    printf("%d checks, %d failures\n",checks,failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
