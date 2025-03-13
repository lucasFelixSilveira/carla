T io = struct({
  public:
  void println = ([]byte str) {
    int32 len = carla?:strlen(str);
    for int32 index : 0..len {
      ascii character = str[index]; 
      carla?:putc(character);
    }
  }
} impl) {

};