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
    std::string command = "";
    std::string arguments = "";

    // parsing: <command> [option] [argument]
    std::cin >> command;
    if (command == "exit"){
      break;
    }
    // remaining arguments w/o 1st space
    getline(std::cin >> std::ws, arguments);

    if (command == "echo"){
      std::cout << arguments << std::endl;
    }
    else{
      std::cout << command << ": command not found" << std::endl;
    }
  }

  return 0;
}
