type string = (_:i8);

extern i32 puts = (string) cut

i32 main = () {

  string msg = "Testando"
  string segundo = "Second"
  i32 counter = 0
  i8 test = 2;
  i16 mds = 4;
  while counter < mds {
    if counter != test {
      puts msg cut
    } else {
      puts segundo cut
    }
    counter += 1
  }
  return test
}