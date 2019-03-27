include_directories(${CMAKE_CURRENT_BINARY_DIR}/stats)

add_library(stats STATIC measure.cc stats.cc)