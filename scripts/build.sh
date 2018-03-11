#!/bin/bash -e
SRC_PATH="`dirname $0`/.."
cd /usr/local/share/palmdev
ln -s "$SRC_PATH/palm-os-sdk/sdk-5r3" .
cd "$SRC_PATH"
palmdev-prep -d sdk-5r3
make

