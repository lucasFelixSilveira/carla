type string = (_:i8);

extern i32 puts = (string) cut

i32 main = (i32 argv, (_:string) args) {


  i32 x = 2;
  for i : x..0 {
    string msg = (i:args);
    puts msg cut
  }

  return 0
}