CC	 = gcc

all: server client

server:
	$(CC) server.c -o server

client:
	$(CC) client.c -o client

clean:
	make all
	rm client
	rm server