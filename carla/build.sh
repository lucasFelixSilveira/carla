#!/bin/bash

echo "Compiling.."

recursive() {
  local directory="$1"

  for file in "$directory"/*; do
    if [ -d "$file" ]; then
      recursive "$file"
    elif [ "${file##*.}" == "c" ]; then
      gcc -c -ggdb "$file" -o "${file%.c}.o"
    fi
  done
}

recursive "./src"

object_files=$(find ./src -name '*.o')

gcc $object_files -debug -dr_debug -ggdb -o ./carla.exe

echo "Compiled!"
echo
