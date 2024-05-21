CC = g++ -std=c++11 -Wall -Wextra -g

client: client.o
	$(CC) $^ -o $@

client.o: client.cpp
	$(CC) $(CFLAGS) $< -c

run: client
	./client

clean:
	rm -f *.o client
	