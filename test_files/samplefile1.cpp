// test_files/sample.cpp
#include <iostream>
void longFunction() {
  for (int i = 0; i < 35; ++i) {
    std::cout << i << std::endl;
  }
}

int main() {
    std::cout << "Hello from sample.cpp!" << std::endl;
    longFunction();
    return 0;
}
