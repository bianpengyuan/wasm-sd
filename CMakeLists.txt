cmake_minimum_required(VERSION 3.9)
project(wasm_sd)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1 -s LEGALIZE_JS_FFI=0 -s EMIT_EMSCRIPTEN_METADATA=1 -O3 -g3 -emit-llvm-bc")

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/abseil-cpp)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/stats)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/api)
add_library(wasm-sd MODULE stackdriver-wasm.cc api/proxy_wasm_intrinsics.cc)