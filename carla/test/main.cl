#include <stdio>
#include <stdheap>
#include <stdstring>

int32 main = (int16 argc, []byte* argv) {
  []byte buffer = heap::alloc(128);
  []byte first = argv[1];
  []byte second = argv[2];
  string::format(buffer, first, second);
  io::println(buffer);
  return 0;
}