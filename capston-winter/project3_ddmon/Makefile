TARGET = target
CC = gcc
CFLAGS = -shared -DRUNTIME -fPIC -g -rdynamic  
.PHONY: c r clean


compile: target.c ddmon.c ddchck.c
	$(CC) -pthread -g target.c -o target 
	$(CC) $(CFLAGS) ddmon.c -o ddmon.so -ldl
	$(CC) ddchck.c -o ddchck

c:
	./ddchck $(TARGET)

r:
	LD_PRELOAD=./ddmon.so ./$(TARGET)

clean:
	rm -f $(TARGET) ddmon ddmon.so ddchck .ddtrace
