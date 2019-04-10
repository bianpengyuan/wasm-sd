cmake_minimum_required(VERSION 3.9)
project(wasm_sd)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1 -s LEGALIZE_JS_FFI=0 -s EMIT_EMSCRIPTEN_METADATA=1 -O3 -g3")

file(GLOB stats_SRC
        "opencensus/context/internal/*.cc"
        "opencensus/stats/internal/*.cc"
        "opencensus/tags/internal/*.cc"
        "opencensus/trace/internal/*.cc"
#        "opencensus/exporters/stats/stackdriver/internal/*.cc"
        "google/api/*.cc"
        "google/devtools/cloudtrace/v2/*.cc"
        "google/monitoring/v3/*.cc"
        "google/rpc/*.cc"
        )
file(GLOB api_SRC
        "api/*.cc"
        )
add_subdirectory(abseil-cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/api)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(wasm-sd MODULE stackdriver-wasm.cc ${api_SRC} ${stats_SRC})
target_link_libraries(wasm-sd absl::base absl::strings absl::numeric absl::time)