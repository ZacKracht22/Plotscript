#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include <csignal>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "ThreadSafeQueue.hpp"
#include "worker.hpp"

std::thread main_thread; //Global thread


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

//Returns the success int of evaluating an istream's plotscript expression given the input interpreter
int eval_from_stream(std::istream & stream, Interpreter& interp){

  if(!interp.parseStream(stream)){
    error("Invalid Program. Could not parse.");
    return EXIT_FAILURE;
  }
  else{
    try{
      Expression exp = interp.evaluate();
    }
    catch(const SemanticError & ex){
      std::cerr << ex.what() << std::endl;
      return EXIT_FAILURE;
    }	
  }

  return EXIT_SUCCESS;
}

//Returns the success int of evaluating a file's plotscript expression given the input interpreter
int eval_from_file(std::string filename, Interpreter& interp){
      
  std::ifstream ifs(filename);

  if(!ifs){
    error("Could not open file for reading.");
    return EXIT_FAILURE;
  }
  
  return eval_from_stream(ifs, interp);
}

//Evaluates an expression given a terminal flag
int eval_from_command(std::string argexp, Interpreter& interp){

  std::istringstream expression(argexp);

  return eval_from_stream(expression, interp);
}

// A REPL is a repeated read-eval-print loop
void repl(ThreadSafeQueue<std::string>& input_queue, ThreadSafeQueue<std::pair<std::string, Expression>>& output_queue){
  std::pair<std::string, Expression> ret;

  while(!std::cin.eof()){
	  prompt();
	  std::string line = readline();

	  if (line == "%exit") { //%exit kills the kernel
		  input_queue.push("die");
		  break;
	  }
	  else if (line == "%start" && !main_thread.joinable()) { //%start starts a new kernel if it is not active
		  Worker new_worker(&input_queue, &output_queue);
		  main_thread = std::thread(new_worker);
	  }
	  else if (line == "%stop" && main_thread.joinable()) { //%stop stops the kernel if it is active
		  input_queue.push("die");
		  main_thread.join();
	  }
	  else if (line == "%reset" && main_thread.joinable()) { //%reset kills the kernel and starts new if it is active, 
		  input_queue.push("die");
		  main_thread.join();
		  Worker new_worker(&input_queue, &output_queue);
		  main_thread = std::thread(new_worker);
	  }
	  else if (line == "%reset" && !main_thread.joinable()) { //or starts a new one if not active
		  Worker new_worker(&input_queue, &output_queue);
		  main_thread = std::thread(new_worker);
	  }
	  else if (!main_thread.joinable()) { //if not one of the kernel commands and the kernel is not active, error
		  std::cerr << "Error: interpreter kernel not running" << std::endl;
	  }
	  else if (line.empty()) continue;
	  else {

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
}

int main(int argc, char *argv[])
{  
Interpreter interp;

ThreadSafeQueue<std::string> input_queue;
ThreadSafeQueue<std::pair<std::string,Expression>> output_queue;

Worker main_worker(&input_queue, &output_queue);
main_thread = std::thread(main_worker);


  if(argc == 2){
    return eval_from_file(argv[1], interp);
  }
  else if(argc == 3){
    if(std::string(argv[1]) == "-e"){ //-e flag used to interpret an expression given in the terminal command
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
