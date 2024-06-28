upload: compile
	arduino-cli upload -p /dev/ttyUSB0 --input-dir ./build/ . 

compile: build-dir
	arduino-cli compile -b arduino:avr:mega --build-cache-path ./build/cache/ --build-path ./build ./connect-four.ino

upload-uno: compile-uno
	arduino-cli upload -p /dev/ttyUSB0 --input-dir ./build-uno/ . 

compile-uno: build-dir-uno
	arduino-cli compile -b arduino:avr:uno --build-cache-path ./build-uno/cache/ --build-path ./build-uno ./connect-four.ino

build-dir:
	mkdir -p ./build
	mkdir -p ./build/cache

build-dir-uno:
	mkdir -p ./build-uno
	mkdir -p ./build-uno/cache
