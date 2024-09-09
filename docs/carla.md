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

### Functions
```carla
i32 main = (void) {}
```
- Return type -> Name -> Operator(sign) -> (arguments) { ...code... }

#### Arguments struct
```carla
type string = (_:i8);
i32 main = (i32 argv, (_:string) args) {}
```

#### Extern functions (LIBC)
```carla
extern i32 puts((_:i8)) cut
```
- This can be used to import the LIBC function (puts) into your code.

### Function call
```carla
puts variable;
```
- Since the first identifier is the identifier of a function, you can enter other identifiers, where the arguments will be. When anything else, if not an identifier, is identified, the call is cut off and it is finally executed.

### Variables
```carla
i32 status = 0;
```
- Type -> Identifier -> Operator(sign) -> Value


# Code Exemples
## Hello World (using process arguments)
```carla
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
```carla
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
```carla
i32 main = () {
  i8 counter = 0;
  while counter < 4 {
    counter += 1;
  }
}
```

# If and Else
- The conditionals, IF and ELSE can be used just like Rust, however, with a difference, it does not force you to buy the same types, such as: i8 == i8; It allows you to compare different types — as long as they are primitive.
```carla
i32 main = (i32 argv) {
  if argv < 4 {
    -- If Code Block
  } else {
    -- Else Code Block
  }
}
```