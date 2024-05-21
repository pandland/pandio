#!/bin/sh

OUTPUT="./build/server"

prepare() {
  mkdir -p build
}

build() {
  gcc main.c -o "$OUTPUT"
}

prepare && build

if [ "$1" = "--run" ]; then
  echo "Running..."
  $OUTPUT
fi
