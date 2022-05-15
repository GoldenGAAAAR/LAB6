#include <hash.hpp>

int main(int argc, char* argv[]) {
  if (argc == 1) {
    std::cout << "NO JSON FILE PATH" << std::endl;
  } else if (argc == 2) {
    Hash hash(argv[1]);
    hash.start();
  } else if (argc == 3) {
    int temp = (*argv[2] - '0');
    Hash hash(argv[1], temp);
    hash.start();
  } else {
    std::cout << "TOO MUCH ARGUMENTS" << std::endl;
  }
  return 0;
}
