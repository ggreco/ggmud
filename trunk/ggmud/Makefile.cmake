all: linux win32

build/Makefile:
	mkdir -p build
	cmake -H. -Bbuild

build_w32/Makefile:
	mkdir -p build_w32
	cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw32.cmake -H. -Bbuild_w32

build_w64/Makefile:
	mkdir -p build_w64
	cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw-w64.cmake -H. -Bbuild_w64

build_osx/Makefile:
	mkdir -p build_osx
	cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-darwin.cmake -H. -Bbuild_osx

linux: build/Makefile
	make -C build 
	-echo "Realizzato eseguibile Linux"

w64: build_w64/Makefile
	make -C build_w64
	-echo "Realizzato eseguibile WINDOWS 64"

win32: build_w32/Makefile
	make -C build_w32
	-echo "Realizzato eseguibile WINDOWS"

osx: build_osx/Makefile
	make -C build_osx

dist: win32
	makensis ggmud.nsi

clean:
	rm -rf build build_w32 build_osx
