rm -r compiler/libs
mkdir compiler/libs

cp -r morgana/lib/*.hpp compiler/libs

mkdir compiler/libs/morgana

cp -r morgana/lib/morgana/* compiler/libs/morgana
