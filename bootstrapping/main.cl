type string = (_:i8);

extern i32 puts = (string) cut

i32 main = (i32 argv, (_:string) args) {
  if argv > 2 {
    string argument = (1:args);
    puts argument cut
  }
  else {
    string executable = (0:args);
    puts executable cut
  }
  return 1;
}