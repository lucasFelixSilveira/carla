type string = (_:i8);

extern i32 puts = (string) cut

i32 main = () {
  string msg = "teste\0asexo";
  i8 counter = 0;
  while counter < 5 {
    puts msg cut
    counter += 1;
  }
  return 0
}