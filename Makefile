
.PHONY: test
test: build
	cd build && make test

.PHONY: build
build: cmake
	cd build && make

.PHONY: cmake
cmake:
	mkdir -p build && cd build && cmake ..

.PHONY: clean
clean:
	rm -rf build/*
