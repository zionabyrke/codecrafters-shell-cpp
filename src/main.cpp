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

std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    char quote = 0;  // 0 = none, can be '\'' or '"'

    for (char c : line) {
        if (c == '\'' || c == '"') {
            if (quote == 0) {
                quote = c;
            } else if (quote == c) {
                quote = 0;
            } else {
                token += c;
            }
        }
        else if (c == ' ' && quote == 0) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

void echo(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << args[i];
        if (i + 1 < args.size()) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
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

void type(const std::vector<std::string>& args) {
    if (args.empty()){
        return;
    }
    const std::string& command = args[0];

    if (is_builtin(command)) {
        std::cout << command << " is a shell builtin" << std::endl;
        return;
    }
    if (check_path_exec(command)){
        return;
    }
    std::cout << command << ": not found" << std::endl;
}

void execute(const std::string& command, const std::vector<std::string>& args) {
    if (!check_path_exec(command, true)) {
        std::cout << command << ": command not found" << std::endl;
        return;
    }
    std::string cmd = command;
    for (const auto& arg : args) {
        cmd += " ";
        cmd += arg;
    }
    std::system(cmd.c_str());
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

void cd(const std::vector<std::string>& args) {
    fs::path target;

    if (args.empty() || args[0] == "~") {
        target = get_home_directory();
    } else {
        target = fs::path(args[0]);
        if (!target.is_absolute()) {
            target = fs::current_path() / target;
        }
    }

    if (fs::exists(target)) {
        fs::current_path(target);
    } else {
        std::cout << "cd: " << target.string()
                  << ": No such file or directory" << std::endl;
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

    // parsing: <command> [option] [argument]
    if (!std::getline(std::cin, line)){
      break; // input problem
    }
    auto tokens = tokenize(line);
    if (tokens.empty()) continue;

    std::string command = tokens[0];
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    if (command == "exit"){
      break;
    }
    else if (command == "echo"){
      echo(args);
    }
    else if(command == "type"){
      type(args);
    }
    else if(command == "pwd"){
      pwd();
    }
    else if(command == "cd"){
      cd(args);
    }
    else if (!command.empty()){
      execute(command, args);
    }
  }

  return 0;
}
