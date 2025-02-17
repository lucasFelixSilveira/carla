cd test
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

clear
../run.sh

# cd ..
# echo
# read "Press any key to kill the process"