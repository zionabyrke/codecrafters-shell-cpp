#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <vector>

#ifdef _WIN32
  #include <io.h>
  #define access _access
  #ifndef X_OK
    #define X_OK 1
  #endif
  constexpr char PATH_LIST_SEPARATOR = ';';
#else
  #include <unistd.h>
  constexpr char PATH_LIST_SEPARATOR = ':';
#endif

namespace fs = std::filesystem;
const std::string BUILTINS[] = {"exit", "echo", "type"};

std::string remove_command(std::string input){
  // POPS the 1st word(command) from input
  std::stringstream ss(input);
  std::string output = "";
  ss >> output;
  return output;
}

bool is_builtin(const std::string &command){
  for(const auto& builtin: BUILTINS){
    if (builtin == command){
      return true;
    }
  }
  return false;
}

void echo(std::string msg){
  std::cout << msg << std::endl;
}

bool check_path_exec(const std::string &command, const char *PATH) {
    if (PATH == nullptr){ 
      return false;
    }
    std::stringstream ss(PATH);
    std::string dir;

    while (std::getline(ss, dir, PATH_LIST_SEPARATOR)) {
        if (dir.empty()){
          continue; // empty directory
        }
        fs::path test_dir = fs::path(dir) / command;
        if (!fs::exists(test_dir)){
          continue;
        }
        if (!fs::is_regular_file(test_dir)){
          continue;
        }
// for linux
#ifndef _WIN32
        if (access(test_dir.c_str(), X_OK) != 0) continue;
#endif
        std::cout << command << " is " << test_dir.string() << std::endl;
        return true;
    }
    return false;
}

void type(std::string command, const char* PATH){
  if (is_builtin(command)){
    std::cout << command << " is a shell builtin" << std::endl;
    return;
  }
  if (check_path_exec(command, PATH)){
    return; // success
  }
  // unsuccessful
  std::cout << command << ": not found" << std::endl;
}

int main() {
  
  // REPL
  while (1)
  {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    std::cout << "$ ";

    // variables
    const char*PATH = std::getenv("PATH");
    std::string line = "";
    std::string command = "";
    std::string args = "";

    // parsing: <command> [option] [argument]
    if (!std::getline(std::cin, line)){
      break; // input problem
    }
    // command
    std::istringstream iss(line);
    iss >> command;
    //args
    std::getline(iss, args); // remove space
    if (!args.empty() && args[0] == ' ') args.erase(0, 1);
    
    if (command == "exit"){
      break;
    }
    else if (command == "echo"){
      echo(args);
    }
    else if(command == "type"){
      std::string next_command = remove_command(args);
      type(next_command, PATH);
    }
    else if (! command.empty()){
      std::cout << command << ": command not found" << std::endl;
    }
  }

  return 0;
}
