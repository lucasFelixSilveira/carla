./build.sh
mv ./carla.exe ./test

cd test

read -p "Press any key to run file"
clear
./carla.exe main.cl

sleep 1
clang ./target/out/ir.ll -o ./target/out/out.exe

echo
echo
read -p "Compiler: ---- Status code: [$?] ---- Press any key to test the LLVM binary"
echo 
echo "Output exe stdout:"
echo



../run.sh

cd ..