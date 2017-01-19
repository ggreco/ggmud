all: linux win32

VERSION := $(shell grep VERSION config.h | cut -d\" -f2 )

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

dist: w64
	makensis ggmud.nsi
	mv GGMud-setup.exe GGMud-win64-$(VERSION).exe

dmg:
	mkdir -p dest
	gtk-mac-bundler app.bundle
	mv ~/Desktop/ggmud.app dest/GGMud.app
	cp README dest/README.txt
	hdiutil makehybrid -hfs -hfs-volume-name GGMud -hfs-openfolder dest dest -o TMP.dmg
	hdiutil convert -format UDZO TMP.dmg -o GGMud-OSX-$(VERSION).dmg
	rm -rf TMP.dmg dest

run:
	mkdir mkself_install
	cp -r leu mkself_install
	cp setup.sh build/ggmud gg_help.txt ggmud.png ggmud.desktop mkself_install
	rm -rf mkself_install/leu/.svn
	makeself mkself_install ggmud-X.Y.Z-linux-x86.run "GGMud self-extractable installer" ./setup.sh
	rm -rf mkself_install

clean:
	rm -rf build build_w32 build_osx
