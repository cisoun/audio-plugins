TARGET    = $(shell uname | tr '[:upper:]' '[:lower:]')
CC_FLAGS  = -I $(LIB_DIR) -std=gnu11 -Wall
BUILD_DIR = build
HB_DIR    = /opt/homebrew
LIB_DIR   = lib

.DEFAULT_GOAL := $(TARGET)

$(info OS is $(TARGET))

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB_DIR)/pugl/build_$(TARGET):
	scripts/build_pugl.sh

darwin: $(LIB_DIR)/pugl/build_$(TARGET) $(BUILD_DIR)
	cc $(CC_FLAGS) \
	   $(LIB_DIR)/pugl/build_darwin/libpugl_mac_cairo-0.0.dylib.p/src_mac_cairo.m.o \
	   $(LIB_DIR)/pugl/build_darwin/libpugl_mac-0.0.dylib.p/*.o \
	   -I $(LIB_DIR) \
	   -I $(LIB_DIR)/pugl/include \
	   -I $(LIB_DIR)/pugl/src \
	   -I $(HB_DIR)/include \
	   -L $(HB_DIR)/lib \
	   -framework AppKit \
	   -framework CoreVideo \
	   -lglfw \
	   $(HB_DIR)/lib/libcairo.dylib \
	   main.c lib/ui/*.c -o $(BUILD_DIR)/test

linux: $(LIB_DIR)/pugl/build_$(TARGET) $(BUILD_DIR)
	cc $(CC_FLAGS) \
	   $(LIB_DIR)/cairo/build/src/libcairo.so \
	   $(LIB_DIR)/pugl/build_linux/libpugl_x11-0.so.0.5.3.p/*.o \
	   $(LIB_DIR)/pugl/build_linux/libpugl_x11_cairo-0.so.0.5.3.p/*.o \
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
