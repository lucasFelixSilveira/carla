#include <stdio>

int32 main = (int16 argc, []byte* argv) {
  []ascii msg = argv[1];
  io::println(msg);
  int8 first = msg[0];
  return first;
}