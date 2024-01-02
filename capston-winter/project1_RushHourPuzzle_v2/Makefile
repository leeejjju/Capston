TEST = "test/board1.txt"

make:
	gcc -o a new.c `pkg-config --libs --cflags gtk+-2.0`

r:
	gcc -o game rushhour_v2.c `pkg-config --libs --cflags gtk+-2.0`

run:
	./game $(TEST)

clean:
	rm a game
