#include <iostream>
#include <string>

int main() {
  
  // REPL
  while (1)
  {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout << "$ ";

    std::string input = "";
    std::cin >> input;
    std::cout << input << ": command not found" << std::endl;
  }
}
