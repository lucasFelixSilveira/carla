#include <stdio>
#include <stdheap>
#include <stdstring>

byte* hello = ([]byte msg) {
  []byte buffer = heap::alloc(128);
  string::format(
    buffer, "That is a '{s}' in Carla", 
    msg 
  );
  return buffer;
}

int32 main = () {
  byte* msg = super::hello("Hello, world");
  io::println(msg);
  heap::dump(msg);
  return 0;
}