TARGET    = $(shell uname | tr '[:upper:]' '[:lower:]')
CC_DEBUG_FLAGS = -O3 -g -fsanitize=address -fno-omit-frame-pointer -fstrict-aliasing
# Add -Wextra for extra paranoia.
CC_FLAGS  = -I $(LIB_DIR) -std=gnu11 -Wall $(LDFLAGS_EXTRA) $(CC_DEBUG_FLAGS)
BUILD_DIR = build
BREW_DIR  = /opt/homebrew
LIB_DIR   = lib

.DEFAULT_GOAL := $(TARGET)

$(info OS is $(TARGET))

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB_DIR)/pugl/build_$(TARGET):
	scripts/build_pugl.sh

darwin: $(LIB_DIR)/pugl/build_$(TARGET) $(BUILD_DIR)
	cc $(CC_FLAGS) \
	   $(LIB_DIR)/pugl/build/libpugl_mac_cairo-0.0.dylib.p/src_mac_cairo.m.o \
	   $(LIB_DIR)/pugl/build/libpugl_mac-0.0.dylib.p/*.o \
	   -I $(LIB_DIR) \
	   -I $(LIB_DIR)/pugl/include \
	   -I $(LIB_DIR)/pugl/src \
	   -I $(BREW_DIR)/include \
	   -L $(BREW_DIR)/lib \
	   -framework AppKit \
	   -framework CoreVideo \
	   -lglfw \
	   -lsndfile \
	   -lpthread \
	   $(BREW_DIR)/lib/libcairo.dylib \
	   main.c lib/ui/*.c lib/*.c -o $(BUILD_DIR)/test

linux: $(LIB_DIR)/pugl/build_$(TARGET) $(BUILD_DIR)
	cc $(CC_FLAGS) \
	   $(LIB_DIR)/cairo/build/src/libcairo.so \
	   $(LIB_DIR)/pugl/build/libpugl_x11-0.so.0.5.3.p/*.o \
	   $(LIB_DIR)/pugl/build/libpugl_x11_cairo-0.so.0.5.3.p/*.o \
	   -I $(LIB_DIR)/cairo/build/src \
	   -I $(LIB_DIR)/cairo/src \
	   -I $(LIB_DIR)/pugl/include \
	   -lX11 \
	   -lXext \
	   main.c -o $(BUILD_DIR)/test

clean:
	rm -rf $(BUILD_DIR)

run: $(BUILD_DIR)
	$(BUILD_DIR)/test
