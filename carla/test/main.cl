#include <stdio>
#include <stdheap>
#include <stdstring>

byte* test = ([]byte name, int16 age) {
  []byte buffer = heap::alloc(128);
  string::format(
    buffer, "{s}! My name is {s} and I am {d} old", 
    "Hello, World", name, age
  );
  return buffer;
}

int32 main = () {
  int32 age = 15;
  byte* msg = super::test("Lucas", age);
  io::println(msg);
  heap::dump(msg);
  return 0;
}