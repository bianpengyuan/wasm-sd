#!/bin/bash
git submodule init
emconfigure cmake -DCMAKE_CXX_COMPILER="$HOME"/emsdk/emscripten/1.38.25/em++ -DBUILD_TESTING=OFF .

emmake make
em++ -s WASM=1 -g3 -s WASM=1 -s LEGALIZE_JS_FFI=0 -s BINARYEN_TRAP_MODE='clamp' -s ALLOW_MEMORY_GROWTH=1 \
-s EMIT_EMSCRIPTEN_METADATA=1 -s TOTAL_MEMORY=524288000 -O3 --js-library api/wasm/cpp/proxy_wasm_intrinsics.js \
libwasm-sd.so api/wasm/cpp/libprotobuf.bc -o wasm-sd.js

wavm-disas wasm-sd.wasm wasm-sd.wat
wavm-compile wasm-sd.wasm wasm-sd.wasm
rm wasm-sd.js wasm-sd.wast libwasm-sd.so
