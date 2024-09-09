type string = (_:i8);

extern i32 puts = (string) cut

i32 main = () {

  string first = "Running..."
  string second = "Second!"
  
  i32 counter = 0
  i8 delimiter = 2;
  i16 limit = 4;
  
  while counter < limit {
    if counter != delimiter {
      puts first cut
    } else {
      puts second cut
    }
    counter += 1
  }

  return 0
}