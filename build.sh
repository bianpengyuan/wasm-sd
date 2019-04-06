#!/bin/bash
emconfigure cmake -DCMAKE_CXX_COMPILER="$HOME"/emsdk/emscripten/1.38.25/em++ \
-DCMAKE_CC_COMPILER="$HOME"/emsdk/emscripten/1.38.25/emcc -DBUILD_TESTING=OFF

emmake make
em++ -s WASM=1 -g3 -s WASM=1 -s LEGALIZE_JS_FFI=0 \
-s EMIT_EMSCRIPTEN_METADATA=1 -O3 --js-library api/proxy_wasm_intrinsics.js \
libwasm-sd.so -o wasm-sd.js

wavm-disas wasm-sd.wasm wasm-sd.wat
wavm-compile wasm-sd.wasm wasm-sd.wasm
rm wasm-sd.js wasm-sd.wast libwasm-sd.so
bash generate_wasm.sh
