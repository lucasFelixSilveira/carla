#include <stdio>
#include <stdheap>
#include <stdstring>

int32 main = (int16 argc, []byte* argv) {
  -- Collect arguments 1 and 2
  []byte first = argv[1];
  []byte second = argv[2];
  []byte msg = "Hello, world!"; 

  -- Allocates 128 bytes as the destination for joining the values ​​of arguments 1 and 2
  []byte buffer = heap::alloc(128);

  -- Join the string of argument 1 with that of 2 (If 1 has placeholders).
  string::format(buffer, first, second);

  -- Print the arguments already together according to the placeholders
  io::println(buffer);
  
  -- Clears the memory reserved for the buffer
  heap::dump(buffer);

  -- Prints the content after cleaning (Lost content)
  io::println(buffer);
  
  return 0;
}