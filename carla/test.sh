./build.sh
mv ./carla.exe ./test

cd test

read -p "Press any key to run file"
clear
cp main.crl main.cl
sleep 1
./carla.exe main.cl
rm main.cl

sleep 1
clang ./target/out/ir.ll -o ./target/out/out.exe      \
  -O3 -march=native -mtune=native -ftree-slp-vectorize \
  -fvisibility=hidden -ftree-vectorize -fno-math-errno  \
  -fomit-frame-pointer -ffast-math -fopenmp -fvectorize  \
  -fprofile-arcs -ftest-coverage -mavx512f -mavx2         \
  -Ofast -funroll-loops

clang ./target/out/ir.ll -S -emit-llvm -o ./target/out/out.ll       \
  -Ofast -march=native -mtune=native -ftree-vectorize  \
  -fno-math-errno -ffast-math -fopenmp -funroll-loops    \
  -mllvm --unroll-threshold=16 -mllvm --slp-vectorize-hor  \
  -mllvm -vectorize-loops -mllvm --allow-unroll-and-jam   \
  -mllvm --lsr-drop-solution -mllvm --mgpopt -funroll-loops

echo
echo
read -p "Compiler: ---- Status code: [$?] ---- Press any key to test the LLVM binary"
echo 
echo "Output exe stdout:"
echo



../run.sh

cd ..