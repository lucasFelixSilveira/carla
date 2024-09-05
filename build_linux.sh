#!/bin/bash

echo "Compiling.."

cfiles=$(find src/ -name '*.c')
for file in $cfiles; do
	echo $file
	gcc -c -ggdb $file -o ${file%.c}.o
done

object_files=$(find ./src -name '*.o')

gcc $object_files -ggdb -o ./carla || exit 1

echo "Compiled!"
echo
