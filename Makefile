.PHONY:all
all:server client 

libnanonet.so:
	g++ -shared -fPIC src/*.cpp -o $@ -std=c++17 -I include/

lib/libnanonet.a:
	g++ -c src/*.cpp -O3 -std=c++17 -static -I include
	mv *.o bin
	ar -rc lib/libnanonet.a bin/*.o

server:server.cpp lib/libnanonet.a
	g++ -o $@ server.cpp -L lib -I include/ -std=c++17 -O3 -lnanonet -static

client:client.cpp lib/libnanonet.a
	g++ -o $@ client.cpp -L lib -I include/ -std=c++17 -O3 -lnanonet -static

test:test.cpp lib/libnanonet.a
	g++ -o $@ test.cpp -L lib -I include/ -std=c++17 -O3 -lnanonet -static

.PHONY:clean
clean:
	rm server client bin/* lib/* test
