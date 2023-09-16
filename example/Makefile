.PHONY:all
all:udp_server udp_client 

udp_server:test_udp_server.cpp
	g++ -o $@ $^ -I ../include -std=c++17 -O3 -static

udp_client:test_udp_client.cpp
	g++ -o $@ $^ -I ../include -std=c++17 -O3 -static

.PHONY:clean
clean:
	rm udp_server udp_client
