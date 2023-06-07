#!/bin/bash

dst=$1

# if no argument, use default path
if [ -z "$1" ]
  then
    dst="./publish"
fi

echo "publish to ${dst}"

mkdir -p ${dst}

cp ./bullet-release/bullet.cocos.js ${dst}/bullet.asm.js

cp ./bullet-release/bullet.wasm.wasm ${dst}/bullet.wasm

cp ./bullet-release/bullet.d.ts ${dst}/bullet.d.ts

echo "Done!"
