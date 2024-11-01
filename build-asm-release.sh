#!/bin/sh
cd bullet
sh autogen.sh
cd ..
python make.py add_func closure release
npm run port2node