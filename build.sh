#!/bin/sh

cd `dirname $0`
base=`pwd`

cc=gcc
cflags="-W -Wall -g -std=c99"
ldflags="-lSDL2 -lGL -lGLEW"
src=$base/1_getting_started/1_hello_window/hello_window.c

[ ! -d "build" ] && mkdir build

cd build

$cc $cflags $src $ldflags -o hello_window
