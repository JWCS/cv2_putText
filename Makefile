#!/usr/bin/make -f
CC=g++
CFLAGS=-std=c++17 -Wall -Wextra -pthread -g -Og -ggdb
LDFLAGS=-I/usr/local/include -L/usr/local/lib `pkg-config --cflags --libs opencv4`
LIBS=-lopencv_core -lopencv_imgproc -lopencv_highgui # Redundant?

.PHONY: run
run: build/test
	cd ./build && ./test && ls *.png

build/test: test.cpp cv2_putText_fancy.hpp cv2_putText.hpp
	mkdir -p build && \
	$(CC) $(CFLAGS) test.cpp -o $@ $(LDFLAGS) $(LIBS)

build/%.png: build/test
	cd ./build && ./test $*
# Note: sometimes, easiest way to see failing tests, if imgs are there,
# is to run `make -k build/*.png`

.PHONY: debug
debug: build/test
	cd ./build && gdb ./test

.PHONY: clean
clean:
	rm -rf ./build/ *.png

build/test_tors: test_tors.cpp
	mkdir -p build && \
	$(CC) $(CFLAGS) test_tors.cpp -o $@ -I/usr/local/include -L/usr/local/lib

.PHONY: run_tors
run_tors: build/test_tors
	cd ./build && ./test_tors

