type string = (_:i8);

extern i32 puts = (string) cut

i32 main = (i32 argv, (_:string) args) {
  string argument = (1:args);
  if argv > 2 {
    puts argument cut;
  }
  else {
    string karalho = (0:args);
    puts karalho cut;
  }
  return 1;
}