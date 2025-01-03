#include <stdio>
#include <stdheap>
#include <stdstring>

int32 main = () {
  []byte buffer = heap::alloc(128);
  string::format(
    buffer, "Hello, world! My name is {s}, and i am {d}",
    "Lucas", 14+1
  );
  io::println(buffer);
  heap::dump(buffer);
  return 0;
}