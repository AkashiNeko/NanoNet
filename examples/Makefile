.PHONY:all
all:udp tcp

udp:udp_client udp_server

tcp:tcp_client tcp_server

udp_server:test_udp_server.cpp
	g++ -o $@ $^ -I ../include -std=c++17 -O3 -static

udp_client:test_udp_client.cpp
	g++ -o $@ $^ -I ../include -std=c++17 -O3 -static

tcp_server:test_tcp_server.cpp
	g++ -o $@ $^ -I ../include -std=c++17 -O3 -static

tcp_client:test_tcp_client.cpp
	g++ -o $@ $^ -I ../include -std=c++17 -O3 -static

.PHONY:clean
clean:
	rm udp_server udp_client tcp_server tcp_client
