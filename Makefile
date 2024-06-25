upload: compile
	arduino-cli upload -p /dev/ttyUSB0 --input-dir ./build/ . 

compile: build-dir
	arduino-cli compile -b arduino:avr:uno --build-cache-path ./build/cache/ --build-path ./build ./connect-four.ino

build-dir:
	mkdir -p ./build
	mkdir -p ./build/cache
