#include <iostream>
#include <string>
#include <sstream>

int main() {
  
  // REPL
  while (1)
  {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout << "$ ";
    std::string input = "";
    std::string command = "";

    // parsing: <command> [option] [argument]
    getline(std::cin, input);

    // command
    std::stringstream ss(input);
    ss >> command;
    if (command == "exit"){
      break;
    }    

    // args: remove command
    size_t fspace_pos = input.find(' ');
    if (fspace_pos != std::string::npos){
      input.erase(0, fspace_pos + 1);
    }
    else{
      input.clear();
    }

    if (command == "echo"){
      std::cout << input << std::endl;
    }
    else if(command == "type"){
      std::cout << input;
      if (input == "exit" || input == "echo" || input == "type"){
        std::cout << " is a shell builtin" << std::endl;
      }
      else{
        std::cout << ": not found" << std::endl;
      }
    }
    else{
      std::cout << command << ": command not found" << std::endl;
    }
  }

  return 0;
}
