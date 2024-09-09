<div align="center">
  <img width="200%" src="../assets/small.png">
  <h1>Carla documentation</h1>
</div>
<br>
<br>
<div align="center">

# Running carla
![running](https://imgur.com/zDDF93A.png)

</div>

# Language structure

### Comments
```lua
-- That's a comment on Carla!
```

### Functions
```zig
i32 main = (void) {}
```
- Return type -> Name -> Operator(sign) -> (arguments) { ...code... }

#### Arguments struct
```zig
type string = (_:i8);
i32 main = (i32 argv, (_:string) args) {}
```

#### Extern functions (LIBC)
```zig
extern i32 puts((_:i8)) cut
```
- This can be used to import the LIBC function (puts) into your code.

### Function call
```lua
puts variable; -- Optional CUT magic word
```
- Since the first identifier is the identifier of a function, you can enter other identifiers, where the arguments will be. When anything else, if not an identifier, is identified, the call is cut off and it is finally executed.

### Variables
```zig
i32 status = 0;
```
- Type -> Identifier -> Operator(sign) -> Value


# Code Exemples
## Hello World (using process arguments)
```zig
type string = (_:i8);

extern i32 puts = (string) cut

i32 main = (i32 argv, (_:string) args) {
  string arg = (1:args);
  puts arg;
  return 0;
}
```
- And run the executable using:
```sh-session
./executable "Hello, world!"
```

## Hello world using variables
```zig
type string = (_:i8);

extern i32 puts = (string) cut

i32 main = () {
  string msg = "Hello, world!"
  puts msg
  return 0
}
```

# While loops
- While loopings work just like any other language. A keyword, a comparison, and a block of code.
```zig
i32 main = () {
  i8 counter = 0;
  while counter < 4 {
    counter += 1;
  }
}
```

# If and Else
- The conditionals, IF and ELSE can be used just like Rust, however, with a difference, it does not force you to buy the same types, such as: i8 == i8; It allows you to compare different types — as long as they are primitive.
```zig
i32 main = (i32 argv) {
  if argv < 4 {
  } else {
  }
}
```

# For loop
- Our forum has a slightly different structure from the others. Having some operators:
  - Ascending order (0, 1, 2, 3...)
```zig
for i..2 {}
```
  - Descending order (...3, 2, 1, 0)
```zig
for 2..i {}
```
  - Specific range (2, 3, 4 / 4, 3, 2)
```zig
for i : 2..4 {}
for i : 4..2 {}
```

```zig
type string = (_:i8);

extern i32 puts = (string) cut

i32 main = () {

  string msg = "Hello, world!"
  for i..2 {
    puts msg cut
  }

  return 0
}
```