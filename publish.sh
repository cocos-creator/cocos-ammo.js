#!/bin/bash
sh ./build-asm-release.sh
sh ./build-asm-debug.sh
sh ./build-wasm-release.sh
sh ./build-wasm-debug.sh

dst=$1

# if no argument, use default path
if [ -z "$1" ]
  then
    dst="./publish"
fi

echo "publish to ${dst}"

mkdir -p ${dst}

cp ./builds/bullet.debug.asm.js ${dst}/bullet.debug.asm.js

cp ./builds/bullet.release.asm.js ${dst}/bullet.release.asm.js

cp ./builds/bullet.debug.wasm.js ${dst}/bullet.debug.wasm.js

cp ./builds/bullet.debug.wasm.wasm ${dst}/bullet.debug.wasm.wasm

cp ./builds/bullet.release.wasm.js ${dst}/bullet.release.wasm.js

cp ./builds/bullet.release.wasm.wasm ${dst}/bullet.release.wasm.wasm

cp ./builds/bullet.d.ts ${dst}/bullet.d.ts

echo "Done!"
