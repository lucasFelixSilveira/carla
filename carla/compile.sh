cd test
clear
./carla.exe main.cl
sleep 1
clang ./target/out/ir.ll -o ./target/out/out.exe 
clear
../run.sh

cd ..
echo
read "Press any key to kill the process"