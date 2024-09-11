type string = (_:i8);

extern i32 puts = (string) cut

i32 main = (i32 argv, (_:string) args) {

  i8 x = 3;
  for i : 1..x {
    string msg = (i:args);
    puts msg cut
  }

  return 0
}