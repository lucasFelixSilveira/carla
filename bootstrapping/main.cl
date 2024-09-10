type string = (_:i8);

extern i32 puts = (string) cut

i32 main = (i32 argv, (_:string) args) {

  for i : 2..4 {
    string msg = (i:args);
    puts msg cut
  }

  return 0
}