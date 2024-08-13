type string = (_:i8);

extern i32 puts = (string) cut

i32 main = (i32 argv, (_:string) args) {
  string carla = (1:args);
  puts carla;
  return 1;
}