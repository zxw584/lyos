#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. $DIR/utils.sh

pushd "$DIR" > /dev/null

if [ ! -d "sources" ]; then 
    mkdir sources
fi

pushd sources > /dev/null

echo "Donwloading packages..."
download "newlib" "ftp://sourceware.org/pub/newlib" "newlib-2.0.0.tar.gz" || cmd_error
download "dash" "http://gondor.apana.org.au/~herbert/dash/files/dash-0.5.8.tar.gz" || cmd_error

echo "Decompressing packages..."
unzip "newlib-2.0.0.tar.gz" "newlib-2.0.0"
unzip "dash-0.5.8.tar.gz" "dash-0.5.8"

echo "Patching..."
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