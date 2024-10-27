#include <stdio>

int32 main = (int32 argc, []byte* argv) {
  []byte msg = argv[1];
  io::println(msg);
  return 0;
}