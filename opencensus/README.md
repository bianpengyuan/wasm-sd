# Opencensus Library

This folder includes opencensus library vendored from <https://github.com/census-instrumentation/opencensus-cpp> with heavy customization. Even though opencensus-cpp repo has already had bazel setup, the original code won't work out of box within a wasm sandbox or in an envoy worker silo, so we have to copy it here with some modification.

The library is mainly used to do data aggregation for some predefined metrics and export them to Stackdriver. The code in this directory should mostly remain unchanged unless there is bug in it.