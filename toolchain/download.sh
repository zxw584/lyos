#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. $DIR/utils.sh

pushd "$DIR" > /dev/null

if [ ! -d "sources" ]; then 
    mkdir sources
fi

pushd sources > /dev/null

echo "Donwloading packages..."
download "binutils" "https://mirrors.ustc.edu.cn/gnu/binutils" "binutils-2.31.tar.gz" || cmd_error
download "gcc" "https://mirrors.ustc.edu.cn/gnu/gcc" "gcc-6.4.0.tar.gz" || cmd_error
download "gcc-native" "https://mirrors.ustc.edu.cn/gnu/gcc" "gcc-4.7.3.tar.bz2" || cmd_error
download "newlib" "ftp://sourceware.org/pub/newlib" "newlib-2.0.0.tar.gz" || cmd_error
download "dash" "http://gondor.apana.org.au/~herbert/dash/files/" "dash-0.5.8.tar.gz" || cmd_error

echo "Decompressing packages..."
unzip "binutils-2.31.tar.gz" "binutils-2.31"
unzip "gcc-6.4.0.tar.gz" "gcc-6.4.0"
unzip "gcc-4.7.3.tar.bz2" "gcc-4.7.3"
unzip "newlib-2.0.0.tar.gz" "newlib-2.0.0"
unzip "dash-0.5.8.tar.gz" "dash-0.5.8"

echo "Patching..."
patc "binutils-2.31"
patc "gcc-6.4.0"
patc "gcc-4.7.3"
patc "newlib-2.0.0"
patc "dash-0.5.8"

echo "Installing extra files..."
install_newlib "newlib-2.0.0"

popd > /dev/null

if [ ! -d "local" ]; then
    mkdir local
fi

if [ ! -d "binary" ]; then
    mkdir binary
fi

popd > /dev/null
