CC = afl-gcc
F = afl-fuzz
AFL = https://github.com/AFLplusplus/AFLplusplus.git

toy:
	$(CC) new.c -o a

toy-fuzz:
	$(F) -i input -o output ./a

libxml:
	$(CC) ./fuzzing-lab/libxml2/configure --disable-shared && make

libxml-fuzz:



