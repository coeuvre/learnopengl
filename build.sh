#!/bin/bash

cd $(dirname $0)
base=$(pwd)

cc=gcc
cflags="-W -Wall -g -std=c99 -I$base"
ldflags="-lm -lSDL2 -lGLEW -lSOIL"
srcs=(
    $base/1_getting_started/1_hello_window/hello_window.c
    $base/1_getting_started/2_hello_triangle/hello_triangle.c
    $base/1_getting_started/3_shaders/shaders.c
    $base/1_getting_started/4_textures/textures.c
)

case $OSTYPE in
    darwin*)
        ldflags="$ldflags -framework OpenGL"
    ;;

    linux*)
        ldflags="$ldflags -lGL"
    ;;

    *)
        echo "UNKNOWN: $OSTYPE"
        exit
    ;;
esac

[ ! -d "build" ] && mkdir build

cd build

for src in ${srcs[@]}
do
    out=$(basename $src)
    out=${out%.*}
    $cc $cflags $src $ldflags -o $out
done
