CC     = gcc
CFLAGS = -g -Wall -Wextra
SRC = src/ministream.c src/bellek_izci.c src/hash_map.c src/linked_list.c src/csv_oku.c

# ── Hedefler ──────────────────────────────────
all: test_temel test_bellek benchmark test_csv

test_temel: $(SRC) test/test_temel.c
	$(CC) $(CFLAGS) $(SRC) test/test_temel.c -o test_temel

test_bellek: $(SRC) test/test_bellek.c
	$(CC) $(CFLAGS) $(SRC) test/test_bellek.c -o test_bellek

benchmark: $(SRC) test/benchmark.c
	$(CC) $(CFLAGS) $(SRC) test/benchmark.c -o benchmark
test_csv: $(SRC) test/test_csv.c
	$(CC) $(CFLAGS) $(SRC) test/test_csv.c -o test_csv

# ── Test & Valgrind ───────────────────────────
test: test_temel test_bellek
	./test_temel
	./test_bellek

valgrind: test_temel
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         ./test_temel

valgrind_bellek: test_bellek
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         ./test_bellek

valgrind_benchmark: benchmark
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         ./benchmark

# ── Temizlik ──────────────────────────────────
clean:
	rm -f test_temel test_bellek benchmark ministream.so
