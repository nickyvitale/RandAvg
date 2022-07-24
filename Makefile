all: dataprocessor.o
	g++ -std=c++1y -o dataproc dataprocessor.o -pthread

dataprocessor.o: dataprocessor.cpp
	g++ -std=c++1y -I./libraries/mqtt_cpp/include -I./libraries/boost_1_79_0 -c dataprocessor.cpp

clean:
	rm *.o dataproc
