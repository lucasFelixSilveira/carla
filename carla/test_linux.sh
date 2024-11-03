./build.sh
mv ./carla ./test

cd test

read -p "Press any key to run file"
clear
./carla main.cl

sleep 1
clang ./target/out/ir.ll -o ./target/out/out

echo
echo
read -p "Compiler: ---- Status code: [$?] ---- Press any key to test the LLVM binary"
echo 
echo "Output exe stdout:"
echo



../run_linux.sh

cd ..