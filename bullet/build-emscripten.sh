#!/usr/bin/env bash
# example: ./build-emscripten.sh copy debug

# ${EMSCRIPTEN} is the path of emscripten sdk. eg: G:\emsdk\upstream\emscripten

# paremeters
thread_num=12 #todo: get thread number from input parameter
copy_dest=../../../../../cocos-engine/native/external/emscripten/bullet
cleanup=0
debug=0
copy=0

# parse input parameters
for var in "$@"
do
    if [ "$var" = "cleanup" ]; then
        cleanup=1
    fi
    if [ "$var" = "debug" ]; then
        debug=1
    fi
    if [ "$var" = "copy" ]; then
        copy=1
    fi
done

echo -e "\033[01;32m --------------- START -------------------- \033[0m"
now=`date +'%Y-%m-%d %H:%M:%S'`
start_time=$(date --date="$now" +%s)

if [ "$cleanup" = 1 ]; then
rm -rf BUILD_WASM
echo "WASM build directory removed"
fi

mkdir BUILD_WASM
echo "WASM build directory created"

cd BUILD_WASM

# mode
if [ "$debug"  = 1 ]; then
mode="debug"
else
mode="MinSizeRel"
fi

echo -e "\033[01;32m --------------- Build $mode --------------- \033[0m"
    cmake .. -G "Unix Makefiles" -B ./ -DBUILD_WASM=ON -DCMAKE_BUILD_TYPE=$mode -DCMAKE_TOOLCHAIN_FILE=${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake
    make -j "$thread_num"
    echo -e "\033[01;32m ---------- Build wasm $mode DONE ----------  \033[0m"
    
    cmake .. -G "Unix Makefiles" -B ./ -DBUILD_WASM=OFF -DCMAKE_BUILD_TYPE=$mode -DCMAKE_TOOLCHAIN_FILE=${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake
    make -j "$thread_num"
    echo -e "\033[01;32m ---------- Build asm $mode DONE ----------  \033[0m"

if [ "$copy"  = 1 ]; then
    cp -r ../../bullet-release-embind/$mode/bullet.$mode.asm.js $copy_dest
    cp -r ../../bullet-release-embind/$mode/bullet.$mode.wasm.js $copy_dest
    cp -r ../../bullet-release-embind/$mode/bullet.$mode.wasm.wasm $copy_dest
fi
    echo -e "\033[01;32m ------------ Copy Done --------------  \033[0m"

now=`date +'%Y-%m-%d %H:%M:%S'`
end_time=$(date --date="$now" +%s);
echo -e "\033[01;32m Time Used: "$((end_time-start_time))"s  \033[1m"
echo -e "\033[01;32m ------------- END -----------------  \033[0m"