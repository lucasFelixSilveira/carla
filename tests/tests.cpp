#include <iostream>
using namespace std;
int main() { 
  int x = 123;
  auto main = [](int x) -> int {
    auto y = (x == 123) ? [](){} : [](){};
    return 0;
  };
  exit((int) main(x));
}


/*

int main = () {
  return 0;
}

*/