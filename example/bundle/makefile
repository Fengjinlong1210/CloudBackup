#compress:compress.cc bundle.cpp
uncompress:uncompress.cc bundle.cpp
	g++ -o $@ $^ -std=c++11 -lpthread
.PHONY:clean
clean:
	rm -r uncompress
#	rm -r compress