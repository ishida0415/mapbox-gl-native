BUILDTYPE ?= Release
PYTHON ?= python
V ?= 1
PREFIX ?= /usr/local

ifeq ($(shell uname -s), Darwin)
PLATFORM ?= osx
endif
PLATFORM ?= linux

.PHONY: all
all: mbgl-core mbgl-platform mbgl-headless

config.gypi: configure
	./configure

config-ios.gypi: configure
	MASON_PLATFORM=ios ./configure config-ios.gypi

#### Library builds ############################################################

.PHONY: mbgl-core
mbgl-core: build/mbgl/Makefile
	$(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) V=$(V) mbgl-core

.PHONY: mbgl-platform
mbgl-platform: build/mbgl/Makefile
	$(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) V=$(V) mbgl-$(PLATFORM)

.PHONY: mbgl-headless
mbgl-headless: build/mbgl/Makefile
	$(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) V=$(V) mbgl-headless

.PHONY: install
install: build/mbgl/Makefile
	$(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) V=$(V) install

#### Build scripts #############################################################

.PHONY: build/mbgl/Makefile
build/mbgl/Makefile: mapboxgl.gyp config.gypi
	deps/run_gyp mapboxgl.gyp -Iconfig.gypi -Dplatform=$(PLATFORM) -Dinstall_prefix=$(PREFIX) --depth=. -Goutput_dir=.. --generator-output=./build/mbgl -f make

.PHONY: build/test/Makefile
build/test/Makefile: test/test.gyp config.gypi
	deps/run_gyp test/test.gyp -Iconfig.gypi -Dplatform=$(PLATFORM) --depth=. -Goutput_dir=.. --generator-output=./build/test -f make

.PHONY: build/linux/Makefile
build/linux/Makefile: linux/mapboxgl-app.gyp config.gypi
	deps/run_gyp linux/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=linux --depth=. -Goutput_dir=.. --generator-output=./build/linux -f make

.PHONY: build/macosx/Makefile
build/macosx/Makefile: macosx/mapboxgl-app.gyp config.gypi
	deps/run_gyp macosx/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=osx --depth=. -Goutput_dir=.. --generator-output=./build/macosx -f make

.PHONY: build/test/test.xcodeproj
build/test/test.xcodeproj: test/test.gyp config.gypi
	deps/run_gyp test/test.gyp -Iconfig.gypi -Dplatform=osx --depth=. -Goutput_dir=.. --generator-output=./build -f xcode

.PHONY: build/macosx/mapboxgl-app.xcodeproj
build/macosx/mapboxgl-app.xcodeproj: macosx/mapboxgl-app.gyp config.gypi
	deps/run_gyp macosx/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=osx --depth=. --generator-output=./build -f xcode

.PHONY: build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj
build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj: ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp config-ios.gypi
	deps/run_gyp ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp -Iconfig-ios.gypi -Dplatform=ios --depth=. --generator-output=./build -f xcode

.PHONY: build/linux/mapboxgl-app.xcodeproj
build/linux/mapboxgl-app.xcodeproj: linux/mapboxgl-app.gyp config.gypi
	deps/run_gyp linux/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=linux --depth=. --generator-output=./build -f xcode

.PHONY: android
android:
	./scripts/local_mason.sh && \
	MASON_DIR=./.mason MASON_PLATFORM=android ./.mason/mason env PATH && \
	export CXX="`MASON_DIR=./.mason MASON_PLATFORM=android ./.mason/mason env CXX`" && \
	export CC="`MASON_DIR=./.mason MASON_PLATFORM=android ./.mason/mason env CC`" && \
	export LD="`MASON_DIR=./.mason MASON_PLATFORM=android ./.mason/mason env LD`" && \
	export LINK="`MASON_DIR=./.mason MASON_PLATFORM=android ./.mason/mason env CXX`" && \
	export AR="`MASON_DIR=./.mason MASON_PLATFORM=android ./.mason/mason env AR`" && \
	export RANLIB="`MASON_DIR=./.mason MASON_PLATFORM=android ./.mason/mason env RANLIB`" && \
	export LDFLAGS="`MASON_DIR=./.mason MASON_PLATFORM=android ./.mason/mason env LDFLAGS` ${LDFLAGS}" && \
	export CFLAGS="`MASON_DIR=./.mason MASON_PLATFORM=android ./.mason/mason env CFLAGS` ${CFLAGS}" && \
	export CPPFLAGS="`MASON_DIR=./.mason MASON_PLATFORM=android ./.mason/mason env CPPFLAGS` ${CPPFLAGS}" && \
	export PATH="`MASON_DIR=./.mason MASON_PLATFORM=android ./.mason/mason env PATH`:${PATH}" && \
	MASON_PLATFORM=android ./configure config-android.gypi && \
	deps/run_gyp android/mapboxgl-app.gyp -Iconfig-android.gypi -Dplatform=android --depth=. --generator-output=./build/android -f make-android && \
	$(MAKE) -C build/android BUILDTYPE=$(BUILDTYPE) V=$(V) androidapp && \
	mkdir -p android/java/lib/src/main/jniLibs/armeabi-v7a && \
	cp build/android/out/$(BUILDTYPE)/lib.target/libmapbox-gl.so android/java/lib/src/main/jniLibs/armeabi-v7a/libmapbox-gl.so && \
	mkdir -p android/java/lib/src/main/assets && \
	cp build/android/out/$(BUILDTYPE)/ca-bundle.crt android/java/lib/src/main/assets/ca-bundle.crt && \
	cp -r build/android/out/$(BUILDTYPE)/styles android/java/lib/src/main/assets/styles && \
	cd android/java && \
	./gradlew build

##### Test cases ###############################################################

test: build/test/Makefile
	$(MAKE) -C build/test BUILDTYPE=$(BUILDTYPE) V=$(V) test

test_%: build/test/Makefile
	$(MAKE) -C build/test BUILDTYPE=$(BUILDTYPE) V=$(V) $*
	(cd build/$(BUILDTYPE) && exec ./test_$*)

# build Mac OS X project for Xcode
xtest: build/test/test.xcodeproj
	open ./build/test/test.xcodeproj

##### Makefile builds ##########################################################


# Builds the linux app with make.
linux: build/linux/Makefile
	$(MAKE) -C build/linux BUILDTYPE=$(BUILDTYPE) V=$(V) linuxapp

# Executes the Linux binary
run-linux: linux
	(cd build/$(BUILDTYPE) && ./mapbox-gl)

# Builds the OS X app with make.
osx: build/macosx/Makefile
	$(MAKE) -C build/macosx BUILDTYPE=$(BUILDTYPE) V=$(V) osxapp

# Executes the OS X binary
run-osx: osx
	build/$(BUILDTYPE)/Mapbox\ GL.app/Contents/MacOS/MAPBOX\ GL

##### Xcode projects ###########################################################

clear_xcode_cache:
    @CUSTOM_DD=`defaults read com.apple.dt.Xcode IDECustomDerivedDataLocation 2>/dev/null`; \
    if [[ $$CUSTOM_DD ]]; then \
        echo clearing files in $$CUSTOM_DD older than one day; \
        find $$CUSTOM_DD/mapboxgl-app-* -mtime +1 | xargs rm -rf; \
    fi; \
    if [[ -d ~/Library/Developer/Xcode/DerivedData/ ]] && [[ ! $$CUSTOM_DD ]]; then \
        echo 'clearing files in ~/Library/Developer/Xcode/DerivedData/mapboxgl-app-* older than one day'; \
        find ~/Library/Developer/Xcode/DerivedData/mapboxgl-app-* -mtime +1 | xargs rm -rf; \
    fi

xproj: build/macosx/mapboxgl-app.xcodeproj
	open ./build/macosx/mapboxgl-app.xcodeproj

iproj: build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj
	open ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj

# build Linux project for Xcode (Runs on Mac OS X too, but without platform-specific code)
lproj: build/linux/mapboxgl-app.xcodeproj
	open ./build/linux/mapboxgl-app.xcodeproj


##### Maintenace operations ####################################################

clean: clear_xcode_cache
	-find ./deps/gyp -name "*.pyc" -exec rm {} \;
	-rm -rf ./build/
	-rm -rf ./macosx/build/
	-rm -rf ./config.gypi ./config-ios.gypi ./config-android.gypi
	-rm -rf ./android/java/build ./android/java/app/build ./android/java/lib/build
	-rm -rf ./android/java/lib/src/main/jniLibs ./android/java/lib/src/main/assets

distclean: clean
	-rm -rf ./mason_packages
