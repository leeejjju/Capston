TEST = "test/map1.txt"

make:
	gcc -o a pushpush.c `pkg-config --libs --cflags gtk+-2.0`

r:
	gcc -o game pushpush.c `pkg-config --libs --cflags gtk+-2.0`

run:
	./game $(TEST)

clean:
	rm a game
