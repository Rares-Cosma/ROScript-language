#!/bin/bash

total_lines=0

files=$(find . -type f \( -name "*.cpp" -o -name "*.h" \))

for file in $files; do
    lines=$(wc -l < "$file")
    total_lines=$((total_lines + lines))
done

echo "Total lines in all .cpp and .h files: $total_lines"
