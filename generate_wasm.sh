#!/bin/bash
generate_yaml(){
    path="$1"/
    if [[ "$1" =~ .*/$ ]]
    then
      path="$1"
    fi

    for file in "$path"*
    do
    if [ -d "$file" ]
    then
         generate_yaml "$file"
    elif [[ "$file" =~ .*\.o$ ]]
    then
         echo "processing $file"
         relative_path=$(echo "$file" | sed 's/.\///')
         dir=`dirname "$relative_path"`
         file=`basename "$relative_path"`
         mkdir -p wat/"$dir"
         new_js_file_name=wat/$dir/$(echo "$file" | cut -f 1 -d '.').js
         new_wasm_file_name=wat/$dir/$(echo "$file" | cut -f 1 -d '.').wasm
         new_wast_file_name=wat/$dir/$(echo "$file" | cut -f 1 -d '.').wast
         new_wat_file_name=wat/$dir/$(echo "$file" | cut -f 1 -d '.').wat
         command="em++ -s WASM=1 -g3 -s LEGALIZE_JS_FFI=0 -s EMIT_EMSCRIPTEN_METADATA=1 -O3 --js-library api/proxy_wasm_intrinsics.js $relative_path -o $new_js_file_name"
         $command
         wavm-disas "$new_wasm_file_name" "$new_wat_file_name"
         rm "$new_js_file_name" "$new_wast_file_name" "$new_wasm_file_name"
    fi
    done
}

rm -R wat
mkdir -p wat
generate_yaml ./
