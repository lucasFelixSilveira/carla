#include "stdio"
#include "stdheap"
#include "stdstring"

int32 main = (int16 argc, []char* argv) {
  
  char* buffer = heap::alloc(128);
  for int32 i : 0..5 {
    string::format(buffer, "{d}. argv[{d}] = {s}", i + 1, i, argv[i]);
    io::println(buffer);
  }

  heap::dump(buffer);
  return 0;
}