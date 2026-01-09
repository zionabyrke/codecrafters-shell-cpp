#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
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
const char*PATH = std::getenv("PATH");
const std::string BUILTINS[] = {"exit", "echo", "type", "pwd", "cd"};

std::string pop_next_word(std::string input){
  // POPS the 1st word(command) from input
  if(input.empty()){
    return "";
  }
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

void echo(const std::string& msg) {
    bool in_single = false;
    bool in_double = false;
    bool last_was_space = false;
    std::string output;

    for (size_t i = 0; i < msg.size(); ++i) {
        char c = msg[i];

        // Toggle single quote only if not inside double quote
        if (c == '\'' && !in_double) {
            in_single = !in_single; //finite automata approach
            continue; // '' discraded
        }

        // Toggle double quote only if not inside single quote
        if (c == '"' && !in_single) {
            in_double = !in_double;
            continue; // "" discarded
        }

        // Space handling
        if (!in_single && !in_double && c == ' '){
            if (!last_was_space) {
                output += ' '; // collapse multiple spaces
                last_was_space = true;
            }
        } else {
            output += c;
            last_was_space = false;
        }
    }

    std::cout << output << std::endl;
}

bool check_path_exec(const std::string &command, bool exec=false) {
    if (PATH == nullptr){ 
      return false;
    }
    std::stringstream ss(PATH);
    std::string dir = "";

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
        if(!exec){
          std::cout << command << " is " << test_dir.string() << std::endl;
        }
        return true;
    }
    return false;
}

void type(std::string command){
  if (is_builtin(command)){
    std::cout << command << " is a shell builtin" << std::endl;
    return;
  }
  if (check_path_exec(command)){
    return; // success
  }
  // unsuccessful
  std::cout << command << ": not found" << std::endl;
}

void execute(std::string command, std::string line){
  if(check_path_exec(command, true)){
    std::system(line.c_str());
  }else{
    std::cout << command << ": command not found" << std::endl;
  }
}

void pwd(){
  std::cout << fs::current_path().string() << std::endl;
}

fs::path get_home_directory() {
    const char* homeDir = nullptr;
    homeDir = std::getenv("HOME"); // Linux/macOS/POSIX
    if (homeDir != nullptr) {
        return fs::path(homeDir);
    } else {
        return fs::path(); 
    }
} 

void cd(std::string dir){
  fs::path fullpath;
  std::string next_arg = pop_next_word(dir);
  if (next_arg == "~"){
    fullpath = get_home_directory();
  }else{
    fullpath = fs::current_path() / dir;
  } 
  
  if(fs::exists(fullpath)){
    fs::current_path(fullpath);
  }else{
    std::cout << "cd: " << dir << ": No such file or directory" << std::endl;
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

    // variables
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
    std::getline(iss, args); // remove space
    if (!args.empty() && args[0] == ' ') args.erase(0, 1);
    
    if (command == "exit"){
      break;
    }
    else if (command == "echo"){
      echo(args);
    }
    else if(command == "type"){
      type(pop_next_word(args));
    }
    else if(command == "pwd"){
      pwd();
    }
    else if(command == "cd"){
      cd(args);
    }
    else if (!command.empty()){
      execute(command, line);
    }
  }

  return 0;
}
