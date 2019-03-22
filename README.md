```
emconfigure cmake .
Update CMAKE_CXX_STANDARD in opencensus-src/CMakeLists.txt to 14
emmake make
em++ libwasm_sd.so \
--js-library /Users/bianpengyuan/go/src/istio.io/wasm-stackdriver/proxy_wasm_intrinsics.js \
-o envoy_filter_http_wasm_example.js
```