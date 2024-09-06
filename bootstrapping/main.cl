type string = (_:i8);

extern i32 puts = (string) cut

i32 main = (i32 argv) {

  string msg = "Testando"
  i8 counter = 0
  while counter < 3 {
    puts msg cut
    counter += 1;
  }

  return 0;
}