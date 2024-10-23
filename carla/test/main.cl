#[import("io")]

int32 main = (int32 argc, []byte* argv) {
  []byte msg = "Hello, World";
  io::println(msg);

  return 0;
}