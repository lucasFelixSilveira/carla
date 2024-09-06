type string = (_:i8);

extern i32 puts = (string) cut

i32 main = () {

  string msg = "Testando"
  string segundo = "Second"
  i8 counter = 0
  while counter < 4 {
    if counter != 2 {
      puts msg cut
    } else {
      puts segundo cut
    }
    counter += 1
  }

  return 0
}