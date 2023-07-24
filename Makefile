.PHONY:all
all:server client 

libnanonet.so:
	g++ -shared -fPIC src/*.cpp -o $@ -std=c++17 -I include/

server:server.cpp libnanonet.so
	g++ -o $@ server.cpp -L ./ -I include/ -std=c++17 -O3 -lnanonet -Wl,-rpath .

client:client.cpp libnanonet.so
	g++ -o $@ client.cpp -L ./ -I include/ -std=c++17 -O3 -lnanonet -Wl,-rpath .

.PHONY:clean
clean:
	rm server client libnanonet.so
