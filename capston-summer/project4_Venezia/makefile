make: venezia.c hamserver.c
	gcc -o a venezia.c -lcurses -pthread
	gcc -o server hamserver.c -pthread
client: venezia.c 
	gcc -o a venezia.c -lcurses -pthread
server: server.c
	gcc -o server server.c -pthread
others: haclient.c gaserver.c hamserver.c
	gcc -o ha haclient.c -lcurses -pthread
	gcc -o ga gaserver.c -pthread
	gcc -o ham hamserver.c -pthread
sample: sample.c
	gcc -o sam sample.c -lcurses
single: single.c
	gcc -o a single.c -lcurses -pthread

