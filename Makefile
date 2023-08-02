.PHONY:all
all:server client 

server:server_demo.cpp
	g++ -o $@ $^ -I include -std=c++17 -O3 -static

client:client_demo.cpp
	g++ -o $@ $^ -I include -std=c++17 -O3 -static

.PHONY:clean
clean:
	rm server client
