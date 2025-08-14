#!/usr/bin/env bash

set -euo pipefail

OUTPUT_HEADER="vulkan_shaders.h"
OUTPUT_SOURCE="vulkan_shaders.cpp"

rm -f "$OUTPUT_HEADER" "$OUTPUT_SOURCE"

echo "#pragma once" >> "$OUTPUT_HEADER"
echo >> "$OUTPUT_HEADER"

generate() {
    local src="$1"
    local spv="${src}.spv"
    local symbol_name
    symbol_name=$(echo "$spv" | tr '.-' '_')

    echo "extern unsigned char ${symbol_name}[];" >> "$OUTPUT_HEADER"
    echo "extern unsigned int ${symbol_name}_len;" >> "$OUTPUT_HEADER"

    echo "/**" >> "$OUTPUT_SOURCE"
    cat "$src" >> "$OUTPUT_SOURCE"
    echo "*/" >> "$OUTPUT_SOURCE"

    glslc "$src" -o "$spv"
    xxd -i "$spv" >> "$OUTPUT_SOURCE"
    echo >> "$OUTPUT_SOURCE"

    rm -f "$spv"
}

for f in \
    drawing_shader.vert \
    drawing_shader.frag \
    font.vert \
    font.frag
do
    generate "$f"
done
