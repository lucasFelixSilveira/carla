./build_linux.sh
mv ./carla.exe ./bin

read -p "Press any key to run file"
clear
./bin/carla

echo
echo
read -p "Compiler: ---- Status code: [$?] ---- Press any key to test the LLVM binary"
./bootstrapping/output
read -p "LLVM: ---- Status code: $?"
