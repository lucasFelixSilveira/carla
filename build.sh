echo "Compiling.."

recursive() {
  local directory="$1"

  for file in "$directory"/*; do
    if [ -d "$file" ]; then
      recursive "$file"
    elif [ "${file##*.}" == "c" ]; then
      gcc -c "$file" -o "${file%.c}.o"
    fi
  done
}

recursive "./src"
gcc ./src/*.o -o ./carla.exe
echo "Compiled!"
echo