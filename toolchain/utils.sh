#!/usr/bin/env bash

function download () {
    echo "Downloading $1 from $2/$3..."
    if [ ! -f "$3" ]; then
        wget "$2/$3"
    else
        echo "$1 already downloaded"
    fi
}

function unzip() {
    echo "Decompressing $1..."
    if [ ! -d "$2" ]; then
        tar -xf $1
    else
        echo "$1 has already been decompressed"
    fi
}

function patc() {
    echo "Patching $1..."
    pushd "$1" > /dev/null
    if [ ! -f ".patched" ]; then
        patch -p1 < $DIR/patches/$1.patch
        touch .patched
    else
        echo "$1 has already been patched"
    fi
    popd > /dev/null
}

function install_newlib () {
    cp -rf $DIR/patches/newlib/lyos $1/newlib/libc/sys/
    cp $DIR/../include/lyos/{type.h,const.h,ipc.h} $1/newlib/libc/sys/lyos/
    cp $DIR/patches/newlib/malign.c $1/newlib/libc/stdlib
    if [ ! -d "$1/newlib/libc/sys/lyos/lyos" ]; then
        mkdir $1/newlib/libc/sys/lyos/lyos
    fi
    cp $DIR/../include/lyos/list.h $1/newlib/libc/sys/lyos/lyos/
}

function cmd_error() {
    echo -e "\033[1;31mBuild failed.\033[0m"
    exit 1
}
