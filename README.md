```
emconfigure cmake -DCMAKE_CXX_COMPILER=/usr/local/google/home/bianpengyuan/emsdk/emscripten/1.38.25/em++ \
-DCMAKE_CC_COMPILER=/usr/local/google/home/bianpengyuan/emsdk/emscripten/1.38.25/emcc

emmake make
em++ -s WASM=1 -g3 -s WASM=1 -s LEGALIZE_JS_FFI=0 \
-s EMIT_EMSCRIPTEN_METADATA=1 -O3 --js-library proxy_wasm_intrinsics.js \
libwasm_sd.so -o wasm-sd.js

wavm-disas wasm-sd.wasm wasm-sd.wat
wavm-compile wasm-sd.wasm wasm-sd.wasm
wavm-run wasm-sd.wasm
```