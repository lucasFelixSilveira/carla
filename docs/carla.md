<div align="center">
  <img width="200%" src="../assets/small.png">
  <h1>Carla documentation</h1>
</div>
<br>
<br>
<div align="center">

# Running carla
<img src="https://imgur.com/fYPOwrC.png" height="500px">

</div>

# Language structure

### Comments
```lua
-- That's a comment on Carla!
```

### Functions
```carla
int32 main = () {}
```
- Return type -> Name -> Operator(sign) -> (arguments) { ...code... }

#### Arguments struct
```carla
int32 main = (int32 argv, []int8* args) {}
```

### Function call
```carla
super::func(arguments...);

-- Or you can use (if the function has inside of a module):
module::func(arguments...);
```
- Since the first identifier is the identifier of a function, you can enter other identifiers, where the arguments will be. When anything else, if not an identifier, is identified, the call is cut off and it is finally executed.

### Variables
```carla
int32 status = 0;
```
- Type -> Identifier -> Operator(sign) -> Value


# Code Exemples
## Hello World (using process arguments)
```carla
#include <stdio>
int32 main = (int32 argv, []int8* args) {
  []int8 arg = args[1];
  io::println(arg);
  return 0;
}
```
- And run the executable using:
```sh-session
./executable "Hello, world!"
```

## Hello world using variables
```carla
#include <stdio>
int32 main = () {
  []int8 msg = "Hello, world!";
  io::println(msg);
  return 0;
}
```

# While loops
- While loopings work just like any other language. A keyword, a comparison, and a block of code.
```carla
int32 main = () {
  int8 counter = 0;
  while counter < 4 {
    counter += 1;
  }
}
```

# If and Else
- The conditionals, IF and ELSE can be used just like Rust, however, with a difference, it does not force you to buy the same types, such as: int8 == int8; It allows you to compare different types — as long as they are primitive.
```carla
int32 main = (int32 argv) {
  if argv < 4 {
  } else {
  }
}
```

# For loop
- Our forum has a slightly different structure from the others. Having some operators:
  - Ascending order (0, 1, 2, 3...)
```carla
for i..2 {}
```
  - Descending order (...3, 2, 1, 0)
```carla
for 2..i {}
```
  - Specific range (2, 3, 4 / 4, 3, 2)
```carla
int8 x = 2;
int8 y = 4;
-- For loops
for i : 2..4 {}
for i : 4..2 {}
for i : 4..x {}
for i : x..4 {}
for i : x..y {}
```

```carla
#include <stdio>
int32 main = () {

  []int8 msg = "Hello, world!";
  for i..2 {
    io::println(msg);
  }

  return 0;
}
```