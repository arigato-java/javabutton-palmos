#!/bin/bash -e
SRC_PATH="`dirname $0`/.."
cd "$SRC_PATH"
palmdev-prep -d sdk-5r3
make

