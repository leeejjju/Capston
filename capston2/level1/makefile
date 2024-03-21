CC = ../AFLplusplus/afl-clang
FZ = ../AFLplusplus/afl-fuzz


btree.out: btree.c
	$(CC) $^ -o $@

fuzz:
	$(FZ) -i inputs -o outputs ./btree.out

clean:
	rm -f *.out
