#main:main.cc bundle.cpp
.PHONY:main
main:main.cc
	g++ -o $@ $^ -std=c++11 -lpthread -lstdc++fs -ljsoncpp -L./lib -lbundle
.PHONY:clean
clean:
	rm -r main