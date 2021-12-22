PRIV_DIR = $(MIX_APP_PATH)/priv
CVT_COLOR_SO = $(PRIV_DIR)/cvt_color.so

CMAKE_BUILD_DIR ?= $(MIX_APP_PATH)/cmake
C_SRC = $(shell pwd)/c_src
ifdef CMAKE_TOOLCHAIN_FILE
	CMAKE_CONFIGURE_FLAGS=-DCMAKE_TOOLCHAIN_FILE="$(CMAKE_TOOLCHAIN_FILE)"
endif

.DEFAULT_GLOBAL := build

build: $(CVT_COLOR_SO)

$(CVT_COLOR_SO): c_src/cvt_color.cpp c_src/nif_utils.hpp
	@ mkdir -p $(PRIV_DIR)
	@ mkdir -p $(CMAKE_BUILD_DIR)
	@ cd $(CMAKE_BUILD_DIR) && \
		cmake -DC_SRC=$(C_SRC) -DERTS_INCLUDE_DIR=$(ERTS_INCLUDE_DIR) \
			-S $(shell pwd) $(CMAKE_CONFIGURE_FLAGS) && \
		cmake --build . $(CMAKE_BUILD_FLAGS)
	@ cp -a $(CMAKE_BUILD_DIR)/cvt_color.so $(CVT_COLOR_SO)
