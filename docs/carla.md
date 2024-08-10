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
