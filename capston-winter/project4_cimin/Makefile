CC = gcc
FZ = ./AFLplusplus/afl-fuzz 
OUTPUT = fuzz-outputs/
.PHONY: r rt rx rb rp rc rr ro test libxml jsmn balance libpng clean

compile:
	$(CC) cimin.c -o run
	$(CC) test.c -o test


#compile with debug option
d:
	$(CC) cimin.c -o run -DDEBUG

# FUZZING=============================================================================

#compile 
fj:
	$(FZ) -i target/jsmn/testcases -o $(OUTPUT)/jsmn ./target/jsmn/jsondump ./target/jsmn/testcases

fx:
	$(FZ) -i target/libxml2/$() -o $(OUTPUT)/libxml ./target/libxml/$() ./target/libxml2/

fp:
	$(FZ) -i target/jsmn/testcases -o $(OUTPUT)/libpng ./target/jsmn/jsondump ./target/jsmn/testcases


# ====================================================================================

#run with test input
r: 
	./run -i test_crash -m "JJ" -o reduced test 0

#run with test input : timeout case
rt:
	./run -i test_crash -m "JJ" -o reduced test 4

#run with libxml2
rx:
	./run -i target/libxml2/testcases/crash.xml -m "SEGV" -o reduced "target/libxml2/xmllint --recover --postvalid -"
#run with jsmn
rj:
	./run -i  target/jsmn/testcases/crash.json -m "heap-buffer-overflow" -o reduced target/jsmn/jsondump 
#run with balance //infinite roop will be occured
rb:
	./run -i  target/balance/testcases/fail -m "NONE" -o reduced target/balance/balance 
#run with libpng
rp: 
	cp target/libpng/crash.png crashes
	cp target/libpng/libpng . -r
	./run -i crashes/crash.png -m use-of-uninitialized-value -o reduced libpng/test_pngfix

rc:
	cp reduced reduced_crash
	./run -i reduced_crash -m use-of-uninitialized-value -o reduced libpng/test_pngfix

ro:
	./libpng/test_pngfix < crash.png
rr:
	./libpng/test_pngfix < reduced

#make crash test
test:
	./test 1 < test_crash
libxml:
	target/libxml2/xmllint --recover --postvalid - < target/libxml2/testcases/crash.xml
jsmn:
	target/jsmn/jsondump < target/jsmn/testcases/crash.json
balance:
	target/balance/balance < target/balance/testcases/fail
libpng:
	cp target/libpng/crash.png .
	cp target/libpng/libpng -r .
	libpng/test_pngfix < crash.png

clean:
	rm -f run test
