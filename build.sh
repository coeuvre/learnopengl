#!/bin/sh

cd $(dirname $0)
base=$(pwd)

cc=gcc
cflags="-W -Wall -g -std=c99"
ldflags="-lSDL2 -lGL -lGLEW"
srcs=(
    $base/1_getting_started/1_hello_window/hello_window.c
    $base/1_getting_started/2_hello_triangle/hello_triangle.c
)

[ ! -d "build" ] && mkdir build

cd build

for src in ${srcs[@]}
do
    out=$(basename $src)
    out=${out%.*}
    $cc $cflags $src $ldflags -o $out
done
