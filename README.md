```
emconfigure cmake .
Update CMAKE_CXX_STANDARD in opencensus-src/CMakeLists.txt to 14
emmake make

em++ libwasm_sd.so \
-s WASM=1 \
--js-library /Users/bianpengyuan/go/src/istio.io/wasm-stackdriver/proxy_wasm_intrinsics.js \
-o wasm-sd.js

wavm-disas wasm-sd.wasm wasm-sd.wat
wavm-compile wasm-sd.wasm wasm-sd.wasm
wavm-run wasm-sd.wasm
```