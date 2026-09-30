CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -O2
SORT_SRC = src/sort.c src/insertionSort.c src/mergeSort.c src/heapSort.c
HEADERS = src/sort.h src/sortctx.h src/bench.h
.PHONY: all run test results sanitize clean
all: src/main.out
src/main.out: src/main.c src/bench.c $(SORT_SRC) $(HEADERS)
	$(CC) $(CFLAGS) -Isrc -o $@ src/main.c src/bench.c $(SORT_SRC)
tests/test_sort.out: tests/test_sort.c src/bench.c $(SORT_SRC) $(HEADERS)
	$(CC) $(CFLAGS) -Isrc -o $@ tests/test_sort.c src/bench.c $(SORT_SRC)
run: src/main.out
	./src/main.out
test: tests/test_sort.out
	./tests/test_sort.out
results: src/main.out
	./src/main.out --csv > report/results.csv
sanitize:
	$(CC) -std=c17 -Wall -Wextra -g -O1 -fsanitize=address,undefined -Isrc -o tests/sanitize.out tests/test_sort.c src/bench.c $(SORT_SRC)
	./tests/sanitize.out
clean:
	rm -f src/*.out tests/*.out
