ifeq ($(shell uname -s), Darwin)
  HOST_PLATFORM = osx
else
  HOST_PLATFORM = linux
endif

PLATFORM ?= $(HOST_PLATFORM)

ifeq ($(PLATFORM),android)
  CONFIGURE_PLATFORM = android
  CONFIGURE_SUBPLATFORM = $(firstword $(SUBPLATFORM) arm-v7)
  CONFIGURE_INPUT = platform/android/scripts/configure.sh
  PLATFORM_OUTPUT = ./build/android-$(CONFIGURE_SUBPLATFORM)

  GYP_FLAVOR_SUFFIX = -android
  ENV = $(shell MASON_ANDROID_ABI=$(CONFIGURE_SUBPLATFORM) ./platform/android/scripts/toolchain.sh)

else ifeq ($(PLATFORM),qt)
  CONFIGURE_PLATFORM = $(HOST_PLATFORM)
  CONFIGURE_SUBPLATFORM = $(shell uname -m)
  CONFIGURE_INPUT = platform/qt/scripts/configure.sh
  PLATFORM_OUTPUT = ./build/qt-$(HOST_PLATFORM)-$(CONFIGURE_SUBPLATFORM)

  # Cross compilation support
  ENV = $(shell MASON_PLATFORM_VERSION=$(CONFIGURE_SUBPLATFORM) ./platform/qt/scripts/toolchain.sh)

else
  CONFIGURE_PLATFORM = $(PLATFORM)
  CONFIGURE_SUBPLATFORM = $(shell uname -m)
  CONFIGURE_INPUT = platform/$(CONFIGURE_PLATFORM)/scripts/configure.sh
  PLATFORM_OUTPUT = ./build/$(PLATFORM)-$(CONFIGURE_SUBPLATFORM)
endif

# Text formatting
TEXT_BOLD = \033[1m
COLOR_GREEN = \033[32m
COLOR_CYAN = \033[36m
COLOR_PINK = \033[35m
FORMAT_END = \033[0m

# Never remove intermediate files
.SECONDARY:

#### Dependencies ##############################################################

ifneq (,$(wildcard .git/.))
.mason:
	git submodule update --init
else
.mason: ;
endif

CONFIGURE_OUTPUT = $(PLATFORM_OUTPUT)/config.gypi
.NOTPARALLEL: $(CONFIGURE_OUTPUT)
$(CONFIGURE_OUTPUT): $(CONFIGURE_INPUT) .mason configure
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Running configure...$(FORMAT_END)\n"
	$(ENV) ./configure $< $@ $(CONFIGURE_PLATFORM) $(CONFIGURE_SUBPLATFORM)

#### Build files ###############################################################

GYP_FLAGS += -I$(CONFIGURE_OUTPUT)
GYP_FLAGS += -Dcoverage=$(ENABLE_COVERAGE)
GYP_FLAGS += -Dcxx_host=$(CXX_HOST)
GYP_FLAGS += -Goutput_dir=.
GYP_FLAGS += --depth=.
GYP_FLAGS += --generator-output=$(PLATFORM_OUTPUT)

.PHONY: Makefile/__project__
Makefile/__project__: $(CONFIGURE_OUTPUT)
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Recreating project...$(FORMAT_END)\n"
	$(ENV) deps/run_gyp platform/$(PLATFORM)/platform.gyp $(GYP_FLAGS) -f make$(GYP_FLAVOR_SUFFIX)

.PHONY: Ninja/__project__
Ninja/__project__: $(CONFIGURE_OUTPUT)
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Recreating project...$(FORMAT_END)\n"
	$(ENV) deps/run_gyp platform/$(PLATFORM)/platform.gyp $(GYP_FLAGS) -f ninja

#### Build individual targets ##################################################

Makefile/%: Makefile/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target $*...$(FORMAT_END)\n"
	$(ENV) $(MAKE) -j$(JOBS) -C $(PLATFORM_OUTPUT) BUILDTYPE=$(BUILDTYPE) $*

Ninja/%: Ninja/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target $*...$(FORMAT_END)\n"
	$(ENV) deps/ninja/ninja-$(PLATFORM) -C $(PLATFORM_OUTPUT)/$(BUILDTYPE) $*

#### Tidy ######################################################################

Ninja/compdb: OUTPUT=$(PLATFORM_OUTPUT)/$(BUILDTYPE)/compile_commands.json
Ninja/compdb: Ninja/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Writing to $(OUTPUT)$(FORMAT_END)\n"
	$(ENV) deps/ninja/ninja-$(PLATFORM) -C $(PLATFORM_OUTPUT)/$(BUILDTYPE) \
		-t compdb cc cc_s cxx objc objcxx > $(OUTPUT)

tidy: Ninja/compdb
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Generating header files...$(FORMAT_END)\n"
	$(ENV) deps/ninja/ninja-$(PLATFORM) -C $(PLATFORM_OUTPUT)/$(BUILDTYPE) version shaders
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Running tidy...$(FORMAT_END)\n"
	@./scripts/clang-tidy.sh $(PLATFORM_OUTPUT)/$(BUILDTYPE)

#### Run tests #################################################################

run-glfw-app:
	cd $(PLATFORM_OUTPUT)/$(BUILDTYPE) && ./mapbox-glfw

run-qt-app:
	cd $(PLATFORM_OUTPUT)/$(BUILDTYPE) && ./qmapboxgl

run-qt-qml-app:
	cd $(PLATFORM_OUTPUT)/$(BUILDTYPE) && ./qquickmapboxgl

run-valgrind-glfw-app:
	cd $(PLATFORM_OUTPUT)/$(BUILDTYPE) && valgrind --leak-check=full --suppressions=../../../scripts/valgrind.sup ./mapbox-glfw

ifneq (,$(shell which gdb))
  GDB = gdb -batch -return-child-result -ex 'set print thread-events off' -ex 'run' -ex 'thread apply all bt' --args
endif

test-%: Makefile/test
	$(GDB) $(PLATFORM_OUTPUT)/$(BUILDTYPE)/test --gtest_catch_exceptions=0 --gtest_filter=$*

check: Makefile/test
	./scripts/collect-coverage.sh $(PLATFORM_OUTPUT)/$(BUILDTYPE)
