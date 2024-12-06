#include <stdio>
#include <stdheap>
#include <stdstring>

byte* test = ([]byte face) {
  []byte buffer = heap::alloc(128);
  string::format(
    buffer, "That is a '{s}' in Carla! {s}", 
    "Hello, World", face
  );
  return buffer;
}

int32 main = () {
  byte* msg = super::test(":3");
  io::println(msg);
  heap::dump(msg);
  return 0;
}