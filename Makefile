.PHONY:all
all:server client 

# libnanonet.so:
# 	g++ -shared -fPIC src/*.cpp -o $@ -std=c++17 -I include/

# lib/libnanonet.a:
# 	g++ -c src/*.cpp -O3 -std=c++17 -static -I include
# 	mv *.o bin
# 	ar -rc lib/libnanonet.a bin/*.o

server:server.cpp
#	g++ -o $@ server.cpp -L lib -I include/ -std=c++17 -O3 -lnanonet -static
	g++ -o $@ server.cpp -I include/ -std=c++17 -O3 -static

client:client.cpp
#	g++ -o $@ client.cpp -L lib -I include/ -std=c++17 -O3 -lnanonet -static
	g++ -o $@ client.cpp -I include/ -std=c++17 -O3 -static

test:test.cpp
#	g++ -o $@ test.cpp -L lib -I include/ -std=c++17 -O3 -lnanonet -static
	g++ -o $@ test.cpp -I include/ -std=c++17 -O3 -static

.PHONY:clean
clean:
	rm server client
