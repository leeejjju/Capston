TEST = "test/map1.txt"

make:
	gcc -o game pushpush.c `pkg-config --libs --cflags gtk+-2.0`
r:
	./game

run:
	GDK_DISABLE_CHECKS=1 ./game
future_run:
	./game $(TEST)

clean:
	rm a game
