#!/bin/sh

set -xe

CC="cc"
CFLAGS="-Wall -Wextra -Werror -Werror=maybe-uninitialized -g -O0 -std=c11 -Wpedantic -Wvla -Wcast-align=strict -Wshadow -Wconversion -Wunreachable-code -Wformat=2 -Wno-incompatible-pointer-types"
CDISABLEDFLAGS="-Wno-error=unused-variable"
# CFILES="metadata.c strings.c slice.c main.c"
CFILES="str.c main.c"
PROGRAM="-o dahon"

$CC $CFLAGS $CDISABLEDFLAGS $PKGS $CFILES $PROGRAM

time ./dahon
