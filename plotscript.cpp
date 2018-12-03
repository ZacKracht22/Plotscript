#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "ThreadSafeQueue.hpp"
#include "worker.hpp"

void prompt(){
  std::cout << "\nplotscript> ";
}

std::string readline(){
  std::string line;
  std::getline(std::cin, line);

  return line;
}

void error(const std::string & err_str){
  std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str){
  std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream & stream, Interpreter& interp){

  //Interpreter interp;
  
  if(!interp.parseStream(stream)){
    error("Invalid Program. Could not parse.");
    return EXIT_FAILURE;
  }
  else{
    try{
      Expression exp = interp.evaluate();
      //std::cout << exp << std::endl;
    }
    catch(const SemanticError & ex){
      std::cerr << ex.what() << std::endl;
      return EXIT_FAILURE;
    }	
  }

  return EXIT_SUCCESS;
}

int eval_from_file(std::string filename, Interpreter& interp){
      
  std::ifstream ifs(filename);

  if(!ifs){
    error("Could not open file for reading.");
    return EXIT_FAILURE;
  }
  
  return eval_from_stream(ifs, interp);
}

int eval_from_command(std::string argexp, Interpreter& interp){

  std::istringstream expression(argexp);

  return eval_from_stream(expression, interp);
}

// A REPL is a repeated read-eval-print loop
void repl(ThreadSafeQueue<std::string>& input_queue, ThreadSafeQueue<std::pair<std::string, Expression>>& output_queue){
  //Interpreter interp;
  //eval_from_file(STARTUP_FILE, interp);
  std::pair<std::string, Expression> ret;

  while(!std::cin.eof()){ //edit this
	  prompt();
	  std::string line = readline();
	  if (line.empty()) continue;
	  input_queue.push(line);
	  output_queue.wait_and_pop(ret);

	  if (ret.first.empty()) { //output expression
		  std::cout << ret.second << std::endl;
	  }
	  else { //output error message
		  std::cerr << ret.first << std::endl;
	  }
  }
}

int main(int argc, char *argv[])
{  
Interpreter interp;
ThreadSafeQueue<std::string> input_queue;
ThreadSafeQueue<std::pair<std::string,Expression>> output_queue;
Worker main_worker(&input_queue, &output_queue);
std::thread main_thread(main_worker);

  if(argc == 2){
    return eval_from_file(argv[1], interp);
  }
  else if(argc == 3){
    if(std::string(argv[1]) == "-e"){
      return eval_from_command(argv[2], interp);
    }
    else{
      error("Incorrect number of command line arguments.");
    }
  }
  else{
      repl(input_queue, output_queue);
  }
    
  main_thread.join();

  return EXIT_SUCCESS;
}
