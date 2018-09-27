#include "environment.hpp"

#include <cassert>
#include <cmath>
#include <complex>
#include <iostream>


#include "environment.hpp"
#include "semantic_error.hpp"

/*********************************************************************** 
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs){
  return args.size() == nargs;
}

/*********************************************************************** 
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args){
  args.size(); // make compiler happy we used this parameter
  return Expression();
};

// the default binary procedure always returns an expresison of type None
Expression default_proc_bi(const std::vector<Expression> & args, Environment & env) {
	args.size(); // make compiler happy we used this parameter
	return Expression();
};

Environment::Environment(const Environment & env) {
	envmap = env.envmap;
}

//Procedure to create a list as a vector of expressions
Expression list(const std::vector<Expression> & args) {
	return Expression(args);
};

//Procedure to return the first item of a list.
//Throws a semantic error for arguments not being a list, more than 1 argument, or an empty list
Expression first(const std::vector<Expression> & args) {
	std::vector<Expression> emptyVector;

	if (nargs_equal(args, 1)) {
		if (args[0].head() == Atom("list")) {
			if (args[0] != Expression(emptyVector)) {
				std::vector<Expression> tail = args[0].getTail();
				return Expression(tail[0]);
			}
			else {
				throw SemanticError("Error in call to first, argument is an empty list");
			}
		}
		else {
			throw SemanticError("Error in call to first, argument not a list");
		}
	}
	else {
		throw SemanticError("Error in call to first, need 1 argument");
	}
};

//Procedure to return the second-last items in a list.
//Throws a semantic error for arguments not being a list, more than 1 argument, or an empty list
Expression rest(const std::vector<Expression> & args) {
	std::vector<Expression> emptyVector, returnVector;

	if (nargs_equal(args, 1)) {
		if (args[0].head() == Atom("list")) {
			if (args[0] != Expression(emptyVector)) {
				//iterate through the vector and push back each expression onto the return vector except for the first item
				for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); ++e) {
					if (e != args[0].tailConstBegin()) {
						returnVector.push_back(*e);
					}
				}
				return Expression(returnVector);
			}
			else {
				throw SemanticError("Error in call to rest, argument is an empty list");
			}
		}
		else {
			throw SemanticError("Error in call to rest, argument not a list");
		}
	}
	else {
		throw SemanticError("Error in call to rest, need 1 argument");
	}
};

//Procedure to return the length of a list
//Throws a semantic error for arguments not being a list, more than 1 argument
Expression length(const std::vector<Expression> & args) {
	std::vector<Expression> emptyVector;

	if (nargs_equal(args, 1)) {
		if (args[0].head() == Atom("list")) {
				return Expression(args[0].tailLength());
		}
		else {
			throw SemanticError("Error in call to length, argument not a list");
		}
	}
	else {
		throw SemanticError("Error in call to length, need 1 argument");
	}
};

//Procedure to append an expression onto another
//Throws a semantic error for first argument not being a list, or not having binary arguments
Expression append(const std::vector<Expression> & args) {
	std::vector<Expression> retVector;

	if (nargs_equal(args, 2)) {
		if (args[0].head() == Atom("list")) {
			for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); ++e) {
				retVector.push_back(*e);
			}
			retVector.push_back(args[1]);
		}
		else {
			throw SemanticError("Error in call to append, argument 1 not a list");
		}
	}
	else {
		throw SemanticError("Error in call to append, need 2 arguments");
	}

	return Expression(retVector);
};

//Procedure to join a list onto another
//Throws a semantic error for not binary args or not two lists
Expression join(const std::vector<Expression> & args) {
	std::vector<Expression> retVector;

	if (nargs_equal(args, 2)) {
		if (args[0].head() == Atom("list") && args[1].head() == Atom("list")) {
			for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); ++e) {
				retVector.push_back(*e);
			}
			for (auto e = args[1].tailConstBegin(); e != args[1].tailConstEnd(); ++e) {
				retVector.push_back(*e);
			}
		}
		else {
			throw SemanticError("Error in call to join, argument 1 or 2 not a list");
		}
	}
	else {
		throw SemanticError("Error in call to join, need 2 arguments");
	}

	return Expression(retVector);
};


//Procedure to create a list from the first argument to the second arg with an increment of the third arg
//Throws a semantic error for not 3 args, any args not a number, first not less than second, or negative increment
Expression range(const std::vector<Expression> & args) {
	std::vector<Expression> returnVector;

	if (nargs_equal(args, 3)) {
		if (args[0].isHeadNumber() && args[1].isHeadNumber() && args[2].isHeadNumber()) {
			if (args[0].head().asNumber() < args[1].head().asNumber()) {
				if (args[2].head().asNumber() > 0){
					double begin = args[0].head().asNumber();
					double end = args[1].head().asNumber();
					double increment = args[2].head().asNumber();
					double i = begin;
					while (i <= end) {
						returnVector.push_back(Expression(i));
						i = i + increment;
					}
				}
				else {
					throw SemanticError("Error in call to range, increment must be positive");
				}
			}
			else {
				throw SemanticError("Error in call to range, arg 1 must be less than arg 2");
			}
		}
		else {
			throw SemanticError("Error in call to range, all arguments must be a number");
		}
	}
	else {
		throw SemanticError("Error in call to range, need 3 arguments");
	}

	return Expression(returnVector);
};

Expression add(const std::vector<Expression> & args){
  // check all aruments are numbers, while adding
  double realResult = 0;
  std::complex<double> complexResult(0.0,0.0);
  bool complexFlag = false; //Flag used to check if one of the arguments is a complex number

  //Check each element for a complex number
  for (auto & z : args){
	  if (z.isHeadComplex()){
		  complexFlag = true;
	  }
  }

  for( auto & a :args){
	  //If the argument is a number and no complex numbers are in the arguments, add it as a normal number
    if(a.isHeadNumber() && !complexFlag){
      realResult += a.head().asNumber();  
    }
	//If the arg is a number but complex numbers are in the arguments, add it to a complex result
	else if (a.isHeadNumber() && complexFlag) {
		complexResult += a.head().asNumber();
	}
	else if (a.isHeadComplex()) {
		complexResult += a.head().asComplex();
	}
    else{
      throw SemanticError("Error in call to add, argument not a number");
    }
  }

  //Return a complex number if there was at  least one complex value
  return (complexFlag ? Expression(complexResult) : Expression(realResult));

};

Expression mul(const std::vector<Expression> & args){
 
  // check all aruments are numbers, while multiplying
	double realResult = 1;
	std::complex<double> complexResult(1.0, 0.0);
	bool complexFlag = false;

	//Check for a complex value in the arguments
	for (auto & z : args) {
		if (z.isHeadComplex()) {
			complexFlag = true;
		}
	}

	//Same logic from add method, if a complex value is present in the list, add all the arguments as complex
	for (auto & a : args) {
		if (a.isHeadNumber() && !complexFlag) {
			realResult *= a.head().asNumber();
		}
		else if (a.isHeadNumber() && complexFlag) {
			complexResult *= a.head().asNumber();
		}
		else if (a.isHeadComplex()) {
			complexResult *= a.head().asComplex();
		}
		else {
			throw SemanticError("Error in call to multiply, argument not a number");
		}
	}

	return (complexFlag ? Expression(complexResult) : Expression(realResult));
};

Expression subneg(const std::vector<Expression> & args){

  double realResult = 0;
  std::complex<double> complexResult(0.0, 0.0);
  bool complexFlag = false;

  // If there is one argument, negate it
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber()){
      realResult = -args[0].head().asNumber();
    }
	else if (args[0].isHeadComplex()) {
		complexResult = -args[0].head().asComplex();
		complexFlag = true;
	}
    else{
      throw SemanticError("Error in call to negate: invalid argument.");
    }
  }
  //If there are two arguments, subtract them. If at least on is complex, the answer is complex
  else if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      realResult = args[0].head().asNumber() - args[1].head().asNumber();
    }
	else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex())) {
		complexResult.real(args[0].head().asNumber() - args[1].head().asComplex().real());
		complexResult.imag(complexResult.imag() - args[1].head().asComplex().imag());
		complexFlag = true;
	}
	else if ((args[0].isHeadComplex()) && (args[1].isHeadNumber())) {
		complexResult.real(args[0].head().asComplex().real() - args[1].head().asNumber());
		complexResult.imag(args[0].head().asComplex().imag());
		complexFlag = true;
	}
	else if ((args[0].isHeadComplex()) && (args[1].isHeadComplex())) {
		complexResult = args[0].head().asComplex() - args[1].head().asComplex();
		complexFlag = true;
	}
    else{      
      throw SemanticError("Error in call to subtraction: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
  }

  return (complexFlag ? Expression(complexResult) : Expression(realResult));
};

Expression div(const std::vector<Expression> & args){

  double realResult = 0;  
  std::complex<double> complexResult(0.0, 0.0);
  bool complexFlag = false;

  //Check for only two arguments
  if(nargs_equal(args,2)){
	  //If both args are numbers, divide as normal numbers and return non-complex
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      realResult = args[0].head().asNumber() / args[1].head().asNumber();
    }
	//If one of the arguments are complex, or both, return as complex
	else if ((args[0].isHeadComplex()) && (args[1].isHeadNumber())) {
		complexResult = args[0].head().asComplex() / args[1].head().asNumber();
		complexFlag = true;
	}
	else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex())) {
		complexResult = args[0].head().asNumber() / args[1].head().asComplex();
		complexFlag = true;
	}
	else if ((args[0].isHeadComplex()) && (args[1].isHeadComplex())) {
		complexResult = args[0].head().asComplex() / args[1].head().asComplex();
		complexFlag = true;
	}
    else{      
      throw SemanticError("Error in call to division: invalid argument.");
    }
  }
  else{
	  if (nargs_equal(args, 1) && args[0].isHeadNumber()) {
		  realResult = 1 / args[0].head().asNumber();
	  }
	  else if (nargs_equal(args, 1) && args[0].isHeadComplex()) {
		  complexResult = std::complex<double>(1.0,0.0) / args[0].head().asComplex();
		  complexFlag = true;
	  }
	  else {
		  throw SemanticError("Error in call to division: invalid number of arguments.");
	  }
  }
  return (complexFlag ? Expression(complexResult) : Expression(realResult));
};

Expression sqrt(const std::vector<Expression> & args) {

	double realResult = 0;
	std::complex<double> complexResult(0.0,0.0);
	bool complexFlag = false;

	//Make sure there is just one arguments
	if (nargs_equal(args, 1)) {
		//If the argument is a positive number, return the sqrt as a double
		if ((args[0].isHeadNumber()) && args[0].head().asNumber() >= 0) {
			realResult = sqrt(args[0].head().asNumber());
		}
		//If the argument is a negative number, return the sqrt as complex
		else if (args[0].isHeadNumber() && args[0].head().asNumber() < 0) {
			complexResult.imag(sqrt(-args[0].head().asNumber()));
			complexFlag = true;
		}
		//If the arg is complex, return the sqrt as complex
		else if (args[0].isHeadComplex()) {
			complexResult = sqrt(args[0].head().asComplex());
			complexFlag = true;
		}
		else {
			throw SemanticError("Error in call to sqrt: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to sqrt: invalid number of arguments.");
	}
	return (complexFlag ? Expression(complexResult) : Expression(realResult));
};

/*Function that takes the first argument to the power of the second.
  If one argument or both are complex, return it as complex.
  */
Expression pow(const std::vector<Expression> & args) {

	double realResult = 0;
	std::complex<double> complexResult(0.0, 0.0);
	bool complexFlag = false;

	if (nargs_equal(args, 2)) {
		if ((args[0].isHeadNumber()) && (args[1].isHeadNumber())) {
			realResult = std::pow(args[0].head().asNumber(), args[1].head().asNumber());
		}
		else if ((args[0].isHeadComplex()) && (args[1].isHeadNumber())) {
			complexResult = std::pow(args[0].head().asComplex(), args[1].head().asNumber());
			complexFlag = true;
		}
		else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex())) {
			complexResult = std::pow(args[0].head().asNumber(), args[1].head().asComplex());
			complexFlag = true;
		}
		else if ((args[0].isHeadComplex()) && (args[1].isHeadComplex())) {
			complexResult = std::pow(args[0].head().asComplex(), args[1].head().asComplex());
			complexFlag = true;
		}
		else {
			throw SemanticError("Error in call to pow: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to pow: invalid number of arguments.");
	}
	return (complexFlag ? Expression(complexResult) : Expression(realResult));
};

/*Function that returns the natural log of an argument.
  Throws an error for invalid number of arguments or an arg less than or equal to 0
  */
Expression ln(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if ((args[0].isHeadNumber()) && args[0].head().asNumber() > 0) {
			result = log(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to ln: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to ln: invalid number of arguments.");
	}
	return Expression(result);
};

/*Function that returns the sin of a real number
*/
Expression sin(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if ((args[0].isHeadNumber())) {
			result = sin(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to sin: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to sin: invalid number of arguments.");
	}
	return Expression(result);
};

/*Function that returns the cos of a real number
*/
Expression cos(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if ((args[0].isHeadNumber())) {
			result = cos(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to cos: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to cos: invalid number of arguments.");
	}
	return Expression(result);
};

/*Function that returns the tan of a real number
*/
Expression tan(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if ((args[0].isHeadNumber())) {
			result = tan(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to tan: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to tan: invalid number of arguments.");
	}
	return Expression(result);
};

/*Function that returns the real part of a complex number*/
Expression real(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if ((args[0].isHeadComplex())) {
			result = args[0].head().asComplex().real();
		}
		else {
			throw SemanticError("Error in call to real: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to real: invalid number of arguments.");
	}
	return Expression(result);
};

/*Function that returns the imaginary part of a complex number*/
Expression imag(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if ((args[0].isHeadComplex())) {
			result = args[0].head().asComplex().imag();
		}
		else {
			throw SemanticError("Error in call to imag: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to imag: invalid number of arguments.");
	}
	return Expression(result);
};

/*Function that returns the magnitude of a complex number*/
Expression mag(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if ((args[0].isHeadComplex())) {
			result = abs(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to mag: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to mag: invalid number of arguments.");
	}
	return Expression(result);
};

Expression arg(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if ((args[0].isHeadComplex())) {
			result = arg(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to arg: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to arg: invalid number of arguments.");
	}
	return Expression(result);
};

Expression conj(const std::vector<Expression> & args) {

	std::complex<double> result(0.0,0.0);

	if (nargs_equal(args, 1)) {
		if ((args[0].isHeadComplex())) {
			result = conj(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to conj: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to conj: invalid number of arguments.");
	}
	return Expression(result);
};

//Binary procedure (first arg is a procedure, second a list) to apply a procedure to each element in a list
Expression apply(const std::vector<Expression> & args, Environment & env) {

	if (nargs_equal(args, 2)) {
		if (args[1].head() == Atom("list")) {
			if (env.is_proc(args[0].head()) ) {
					Expression ret(args[0].head());
					for (auto a : args[1].getTail()) {
						ret.append(a.head());
					}

					return ret.eval(env);
			}
			else if (args[0].head() == Atom("lambda")) {
				//get the lambda expression
				Expression lambdaExp = args[0];
				//create new environment
				Environment newEnv = Environment(env);

				//get the list of parameter symbols
				std::vector<Expression> params = lambdaExp.getTail().at(0).getTail();

				//if the size of the arguments and the size of the inputs dont match throw an error
				if (params.size() != args.at(1).getTail().size()) {
					throw SemanticError("Error during evaluation: lambda function called with incorrect number of args");
				}

				//save the inputs as known expressions
				for (int i = 0; i < params.size(); i++) {
					newEnv.add_exp(params[i].head(), args.at(1).getTail().at(i), true);
				}

				//return the evaluation
				return lambdaExp.getTail().at(1).eval(newEnv);

			}
			else {
				throw SemanticError("Error in call to apply: first arg must be a procedure or lambda function");
			}
		}
		else {
			throw SemanticError("Error in call to apply: second arg needs to be list");
		}
	}
	else {
		throw SemanticError("Error in call to apply: invalid number of arguments.");
	}

};


//Binary procedure (first arg is a procedure, second a list) to map a procedure to each element in a list
Expression map(const std::vector<Expression> & args, Environment & env) {
	if (nargs_equal(args, 2)) {
		if (args[1].head() == Atom("list")) {
			if (env.is_proc(args[0].head())) {
				Expression ret(Atom("list"));
				for (auto a : args[1].getTail()) {
					Expression val(args[0].head());
					val.append(a.head());
					val = val.eval(env);
					if (val.isHeadNumber()) {
						ret.append(val.head().asNumber());
					}
					else if (val.isHeadComplex()) {
						ret.append(val.head().asComplex());
					}
				}
				return ret;
			}
			else if (args[0].head() == Atom("lambda")) {
				//get the lambda expression
				Expression lambdaExp = args[0];
				//create new environment
				Environment newEnv = Environment(env);
				//get the list of parameter symbols
				std::vector<Expression> params = lambdaExp.getTail().at(0).getTail();
				std::vector<Expression> inputs = args[1].getTail();

				Expression ret(Atom("list"));
				Expression val;
				for (auto a : inputs) {
					//if the size of the arguments and the size of the inputs dont match throw an error
					if (params.size() != args.at(1).getTail().size()) {
						throw SemanticError("Error during evaluation: lambda function called with incorrect number of args");
					}

					//save the inputs as known expressions
					for (int i = 0; i < params.size(); i++) {
						newEnv.add_exp(params[i].head(), a.getTail().at(i), true);
					}

					//evaluate with that input
					val = lambdaExp.getTail().at(1).eval(newEnv);
					if (val.isHeadNumber()) {
						ret.append(val.head().asNumber());
					}
					else if (val.isHeadComplex()) {
						ret.append(val.head().asComplex());
					}
				}
				return ret;

			}
			else {
				throw SemanticError("Error in call to apply: first arg must be a procedure or lambda function");
			}
		}
		else {
			throw SemanticError("Error in call to apply: second arg needs to be list");
		}
	}
	else {
		throw SemanticError("Error in call to apply: invalid number of arguments.");
	}

};

//////////////////////////////////////////////////////////////////////////////////////////////////// last line of built in procedures

Environment::Environment(){

  reset();
}

bool Environment::is_known(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const{

  Expression exp;
  
  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ExpressionType)){
      exp = result->second.exp;
    }
  }

  return exp;
}

//lambdaFlag: true if being ran from a lambda function, false if not
void Environment::add_exp(const Atom & sym, const Expression & exp, bool lambdaFlag){

  if(!sym.isSymbol()){
    throw SemanticError("Attempt to add non-symbol to environment");
  }

  // error if overwriting symbol map
  if((envmap.find(sym.asSymbol()) != envmap.end()) && !lambdaFlag){
    throw SemanticError("Attempt to overwrite symbol in environemnt");
  }

  if (lambdaFlag) {
	  envmap.erase(sym.asSymbol());
	  envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp));
  }
  else {
	  envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp));
  }
}

bool Environment::is_proc(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom & sym) const{

  //Procedure proc = default_proc;

  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ProcedureType)){
      return result->second.proc;
    }
  }

  return default_proc;
}

bool Environment::is_proc_bi(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ProcedureBiType);
}

Procedure_bi Environment::get_proc_bi(const Atom & sym) const {

	//Procedure proc = default_proc;

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if ((result != envmap.end()) && (result->second.type == ProcedureBiType)) {
			return result->second.proc_bi;
		}
	}

	return default_proc_bi;
}

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> I(0.0, 1.0);

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset(){

  envmap.clear();
  
  // Built-In value of pi
  envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));

  // Built-In value of euler's number
  envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));

  // Built-In value of euler's number
  envmap.emplace("I", EnvResult(ExpressionType, Expression(I)));

  // Procedure: add;
  envmap.emplace("+", EnvResult(ProcedureType, add)); 

  // Procedure: subneg;
  envmap.emplace("-", EnvResult(ProcedureType, subneg)); 

  // Procedure: mul;
  envmap.emplace("*", EnvResult(ProcedureType, mul)); 

  // Procedure: div;
  envmap.emplace("/", EnvResult(ProcedureType, div)); 

  // Procedure: sqrt;
  envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));

  // Procedure: pow;
  envmap.emplace("^", EnvResult(ProcedureType, pow));

  // Procedure: ln;
  envmap.emplace("ln", EnvResult(ProcedureType, ln));

  // Procedure: sin;
  envmap.emplace("sin", EnvResult(ProcedureType, sin));

  // Procedure: cos;
  envmap.emplace("cos", EnvResult(ProcedureType, cos));

  // Procedure: tan;
  envmap.emplace("tan", EnvResult(ProcedureType, tan));

  // Procedure: real;
  envmap.emplace("real", EnvResult(ProcedureType, real));

  // Procedure: imag;
  envmap.emplace("imag", EnvResult(ProcedureType, imag));

  // Procedure: mag;
  envmap.emplace("mag", EnvResult(ProcedureType, mag));

  // Procedure: arg;
  envmap.emplace("arg", EnvResult(ProcedureType, arg));

  // Procedure: conj;
  envmap.emplace("conj", EnvResult(ProcedureType, conj));

  // Procedure: list;
  envmap.emplace("list", EnvResult(ProcedureType, list));

  // Procedure: first;
  envmap.emplace("first", EnvResult(ProcedureType, first));

  // Procedure: rest;
  envmap.emplace("rest", EnvResult(ProcedureType, rest));

  // Procedure: length;
  envmap.emplace("length", EnvResult(ProcedureType, length));

  // Procedure: append;
  envmap.emplace("append", EnvResult(ProcedureType, append));

  // Procedure: join;
  envmap.emplace("join", EnvResult(ProcedureType, join));

  // Procedure: range;
  envmap.emplace("range", EnvResult(ProcedureType, range));

  // Binary Procedure: apply;
  envmap.emplace("apply", EnvResult(ProcedureBiType, apply));

  // Binary Procedure: map;
  envmap.emplace("map", EnvResult(ProcedureBiType, map));
}
