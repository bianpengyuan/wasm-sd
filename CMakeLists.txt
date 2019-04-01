cmake_minimum_required(VERSION 3.9)
project(wasm_sd)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1 -s LEGALIZE_JS_FFI=0 -s EMIT_EMSCRIPTEN_METADATA=1 -O3 -g3")

file(GLOB stats_SRC
        "stats/*.cc"
        )
file(GLOB api_SRC
        "api/*.cc"
        )

message(${stats_SRC})
add_subdirectory(abseil-cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/stats)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/api)
add_library(wasm-sd MODULE stackdriver-wasm.cc ${api_SRC} stats/measure.cc stats/measure_registry.cc stats/measure_registry_impl.cc)
target_link_libraries(wasm-sd absl::base absl::strings absl::numeric)