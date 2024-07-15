./build.sh
mv ./carla.exe ./bin

read -p "Press any key to run file"
clear
./bin/carla.exe

echo
echo
read -p "---- Status code: $?"