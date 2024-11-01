#!/bin/sh
cd bullet
sh autogen.sh
cd ..
python make.py add_func wasm closure release
