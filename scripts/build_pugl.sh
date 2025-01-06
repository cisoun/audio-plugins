#!/bin/bash

set -eu

PUGL_DIR=lib/pugl

if [ ! -d $PUGL_DIR ]; then
	echo "must be ran at project root"
	exit 1
fi

TARGET=$(uname | tr '[:upper:]' '[:lower:]')

BUILD_DIR=build_$TARGET

cd $PUGL_DIR
meson setup $BUILD_DIR
cd $BUILD_DIR
ninja
