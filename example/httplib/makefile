#server:server.cc
client:client.cc
	g++ -o $@ $^ -std=c++11 -lpthread
.PHONY:clean
clean:
	rm -r client
#	rm -r server