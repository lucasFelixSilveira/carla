type string = (_:i8);

extern i32 puts = (string) cut

i32 main = () {

  string msg = "Hello, world!"
  for i..2 {
    puts msg cut
  }

  return 0
}