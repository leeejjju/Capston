CC = gcc
.PHONY: r rt rx rb test libxml jsmn balance clean

compile:
	$(CC) cimin.c -o run -DDEBUG
	$(CC) test.c -o test

#run with test input
r: 
	./run -i test_crash -m "Hello" -o reduced test 0
#run with test input : timeout case
rt:
	./run -i test_crash -m "Hello" -o reduced test 4

#run with libxml2
rx:
	./run -i target/libxml2/testcases/crash.xml -m "SEGV" -o reduced "target/libxml2/xmllint --recover --postvalid -"
#run with jsmn
rj:
	./run -i  target/jsmn/testcases/crash.json -m "heap-buffer-overflow" -o reduced target/jsmn/jsondump 
#run with balance //infinite roop will be occured
rb:
	./run -i  target/balance/testcases/fail -m "NONE" -o reduced target/balance/balance 

#make crash test
test:
	./test 1 < test_crash
libxml:
	target/libxml2/xmllint --recover --postvalid - < target/libxml2/testcases/crash.xml
jsmn:
	target/jsmn/jsondump < target/jsmn/testcases/crash.json
balance:
	target/balance/balance < target/balance/testcases/fail

clean:
	rm -f run test
