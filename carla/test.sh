./build.sh
mv ./carla.exe ./test

cd test

read -p "Press any key to run file"
clear
./carla.exe main.cl

echo
echo
read -p "Compiler: ---- Status code: [$?] ---- Press any key to test the LLVM binary"
echo 
echo "Output exe stdout:"
echo

./out.exe
read -p "LLVM: ---- Status code: $?"

cd ..