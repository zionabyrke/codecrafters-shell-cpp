#include <iostream>
#include <string>
#include <sstream>

const std::string builtins[] = {"exit", "echo", "type"};

std::string remove_command(std::string input){
  // POPS the 1st word(command) from input
    size_t fspace_pos = input.find(' ');
    if (fspace_pos != std::string::npos){
      input.erase(0, fspace_pos + 1);
    }
    else{
      input.clear();
    }
    return input;
}

bool is_builtin(std::string command){
  for(std::string builtin: builtins){
    if (builtin == command){
      return true;
    }
  }
  return false;
}

void echo(std::string msg){
  std::cout << msg << std::endl;
}

void type(std::string command){
  std::cout << command;
  if (is_builtin(command)){
    std::cout << " is a shell builtin" << std::endl;
  }
  else{
    std::cout << ": not found" << std::endl;
  }
}

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
    input = remove_command(input);

    if (command == "exit"){
      break;
    }
    else if (command == "echo"){
      echo(input);
    }
    else if(command == "type"){
      ss >> command;
      type(command);
    }
    else{
      std::cout << command << ": command not found" << std::endl;
    }
  }

  return 0;
}
