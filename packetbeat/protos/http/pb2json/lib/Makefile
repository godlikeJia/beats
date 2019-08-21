CXX=g++
CXXFLAGS=-std=c++11 -Werror=vla -fPIC
protobuf_lib=${GOPATH}/src/RecSys/thirdparty/protobuf-3.6.0


all: build

build: pb2json.cpp
	g++ -c -std=c++11 -Werror=vla -fPIC \
        -o pb2json.o \
        pb2json.cpp \
	    -I ${protobuf_lib}/include/ 
	    #${protobuf_lib}/lib64_release/libprotobuf.a 

	ar -crv libpb2json.a pb2json.o

.PHONY: test
test: 
	g++ -ggdb3 -std=c++11 -Werror=vla \
        -o pb2json \
        pb2json_test.cpp \
        ./libpb2json.a \
	    ${protobuf_lib}/lib64_release/libprotobuf.a \
        -lpthread

install:
	cp ./libpb2json.a ../
	cp ./pb2json.h ../
