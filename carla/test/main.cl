#include <stdio>

int32 main = (int32 argc, []byte* argv) {
  []byte arg = argv[0];
  io::println(arg);
  return 0;
}