cd test
clear
cp main.crl main.cl
sleep 1
./carla.exe main.cl
rm main.cl
sleep 1
clang ./target/out/ir.ll -o ./target/out/out.exe 
clear
../run.sh

cd ..
echo
read "Press any key to kill the process"