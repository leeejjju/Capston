TARGET = target
CC = gcc
CFLAGS = -shared -DRUNTIME -fPIC -g -rdynamic  
.PHONY: check run clean


compile: target.c ddmon.c ddchck.c
	$(CC) -pthread -g target.c -o target
	$(CC) $(CFLAGS) ddmon.c -o ddmon.so -ldl
	$(CC) ddchck.c -o ddchck

check:
	./ddchck $(TARGET)

run:
	LD_PRELOAD=./ddmon.so ./$(TARGET)

clean:
	rm -f $(TARGET) ddmon ddmon.so ddchck .ddtrace
