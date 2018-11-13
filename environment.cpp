#include "environment.hpp"

#include <cassert>
#include <cmath>
#include <complex>
#include <iostream>
#include <string>
#include <iomanip>


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
	env.reset();
	return Expression();
};

// the default procedure_prop always returns an expresison of type None
Expression default_proc_prop(std::vector<Expression> & args) {
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

Expression discrete_plot(std::vector<Expression> & args) {
	Expression SIZE(0.5);
	Expression POINT(Atom("\"point\""));
	Expression THICKNESS(0);
	Expression LINE(Atom("\"line\""));
	Expression TEXT(Atom("\"text\""));

	double x_min = args.at(0).getTail().at(0).getTail().at(0).head().asNumber();
	double x_max = x_min;
	double y_min = args.at(0).getTail().at(0).getTail().at(1).head().asNumber();
	double y_max = y_min;
	
	Expression data = args.at(0);
	Expression options = args.at(1);
	std::vector<Expression> ret;
	std::vector<Expression> points;
	//std::vector<Expression> lines;

	for (auto d : data.getTail()) {
		double x_point = d.getTail().at(0).head().asNumber();
		double y_point = d.getTail().at(1).head().asNumber();

		if (x_point > x_max) x_max = x_point;
		if (x_point < x_min) x_min = x_point;
		if (y_point > y_max) y_max = y_point;
		if (y_point < y_min) y_min = y_point;
	}

	const double xscale = 20 / (x_max - x_min);
	const double yscale = 20 / (y_max - y_min);
	double xmiddle = (x_max + x_min) / 2;
	double ymiddle = (y_max + y_min) / 2;

	for (auto a : data.getTail()) {
		std::vector<Expression> make_point;
		make_point.push_back(Expression(a.getTail().at(0).head().asNumber() * xscale));
		make_point.push_back(Expression(-a.getTail().at(1).head().asNumber() * yscale));
		Expression point = Expression(make_point);
		point.setProperty("\"object-name\"", POINT);
		point.setProperty("\"size\"",SIZE);
		points.push_back(point);

		std::vector<Expression> make_axis_point;
		make_axis_point.push_back(Expression(a.getTail().at(0).head().asNumber() * xscale));
		make_axis_point.push_back(Expression(0));
		Expression axis_point = Expression(make_axis_point);

		std::vector<Expression> make_line;
		make_line.push_back(point);
		make_line.push_back(axis_point);
		Expression line = Expression(make_line);
		line.setProperty("\"object-name\"", LINE);
		line.setProperty("\"thickness\"", THICKNESS);
		ret.push_back(line);
	}

	std::vector<Expression> make_bottom_line_left;
	std::vector<Expression> make_bottom_line_right;
	std::vector<Expression> make_bottom_line;
	make_bottom_line_left.push_back(Expression(x_min*xscale));
	make_bottom_line_left.push_back(Expression(-y_min*yscale));
	make_bottom_line_right.push_back(Expression((x_min*xscale) + 20));
	make_bottom_line_right.push_back(Expression(-y_min*yscale));
	make_bottom_line.push_back(make_bottom_line_left);
	make_bottom_line.push_back(make_bottom_line_right);
	Expression bottom_line = Expression(make_bottom_line);
	bottom_line.setProperty("\"object-name\"", LINE);
	bottom_line.setProperty("\"thickness\"", THICKNESS);
	ret.push_back(bottom_line);

	std::vector<Expression> make_top_line_left;
	std::vector<Expression> make_top_line_right;
	std::vector<Expression> make_top_line;
	make_top_line_left.push_back(Expression(x_min*xscale));
	make_top_line_left.push_back(Expression(-y_max*yscale));
	make_top_line_right.push_back(Expression((x_min*xscale) + 20));
	make_top_line_right.push_back(Expression(-y_max*yscale));
	make_top_line.push_back(make_top_line_left);
	make_top_line.push_back(make_top_line_right);
	Expression top_line = Expression(make_top_line);
	top_line.setProperty("\"object-name\"", LINE);
	top_line.setProperty("\"thickness\"", THICKNESS);
	ret.push_back(top_line);

	std::vector<Expression> make_left_line_bottom;
	std::vector<Expression> make_left_line_top;
	std::vector<Expression> make_left_line;
	make_left_line_bottom.push_back(Expression(x_min*xscale));
	make_left_line_bottom.push_back(Expression(-y_min*yscale));
	make_left_line_top.push_back(Expression(x_min*xscale));
	make_left_line_top.push_back(Expression((-y_min*yscale) - 20));
	make_left_line.push_back(make_left_line_bottom);
	make_left_line.push_back(make_left_line_top);
	Expression left_line = Expression(make_left_line);
	left_line.setProperty("\"object-name\"", LINE);
	left_line.setProperty("\"thickness\"", THICKNESS);
	ret.push_back(left_line);

	std::vector<Expression> make_right_line_bottom;
	std::vector<Expression> make_right_line_top;
	std::vector<Expression> make_right_line;
	make_right_line_bottom.push_back(Expression(x_max*xscale));
	make_right_line_bottom.push_back(Expression(-y_min*yscale));
	make_right_line_top.push_back(Expression(x_max*xscale));
	make_right_line_top.push_back(Expression((-y_min*yscale) - 20));
	make_right_line.push_back(make_right_line_bottom);
	make_right_line.push_back(make_right_line_top);
	Expression right_line = Expression(make_right_line);
	right_line.setProperty("\"object-name\"", LINE);
	right_line.setProperty("\"thickness\"", THICKNESS);
	ret.push_back(right_line);

	if (x_min < 0 && x_max > 0) {
		std::vector<Expression> make_middle_vertical_line_bottom;
		std::vector<Expression> make_middle_vertical_line_top;
		std::vector<Expression> make_middle_vertical_line;
		make_middle_vertical_line_bottom.push_back(Expression(xmiddle*xscale));
		make_middle_vertical_line_bottom.push_back(Expression((-ymiddle*yscale) + 10));
		make_middle_vertical_line_top.push_back(Expression(xmiddle*xscale));
		make_middle_vertical_line_top.push_back(Expression((-ymiddle*yscale) - 10));
		make_middle_vertical_line.push_back(make_middle_vertical_line_bottom);
		make_middle_vertical_line.push_back(make_middle_vertical_line_top);
		Expression middle_vertical_line = Expression(make_middle_vertical_line);
		middle_vertical_line.setProperty("\"object-name\"", LINE);
		middle_vertical_line.setProperty("\"thickness\"", THICKNESS);
		ret.push_back(middle_vertical_line);
	}

	if (y_min < 0 && y_max > 0) {
		std::vector<Expression> make_middle_horizontal_line_left;
		std::vector<Expression> make_middle_horizontal_line_right;
		std::vector<Expression> make_middle_horizontal_line;
		make_middle_horizontal_line_left.push_back(Expression((xmiddle*xscale) - 10));
		make_middle_horizontal_line_left.push_back(Expression(-ymiddle*yscale));
		make_middle_horizontal_line_right.push_back(Expression((xmiddle*xscale) + 10));
		make_middle_horizontal_line_right.push_back(Expression(-ymiddle*yscale));
		make_middle_horizontal_line.push_back(make_middle_horizontal_line_left);
		make_middle_horizontal_line.push_back(make_middle_horizontal_line_right);
		Expression middle_horizontal_line = Expression(make_middle_horizontal_line);
		middle_horizontal_line.setProperty("\"object-name\"", LINE);
		middle_horizontal_line.setProperty("\"thickness\"", THICKNESS);
		ret.push_back(middle_horizontal_line);
	}

	for (auto p : points) {
		ret.push_back(p);
	}

	std::vector<Expression> titleposition;
	titleposition.push_back(Expression(xmiddle*xscale));
	titleposition.push_back(Expression(-y_max*yscale - 3));
	Expression titlepositionexp = Expression(titleposition);

	std::vector<Expression> xlabel;
	xlabel.push_back(Expression(xmiddle*xscale));
	xlabel.push_back(Expression(-y_min*yscale + 3));
	Expression xlabelexp = Expression(xlabel);

	std::vector<Expression> ylabel;
	ylabel.push_back(Expression(x_min*xscale - 3));
	ylabel.push_back(Expression(-ymiddle*yscale));
	Expression ylabelexp = Expression(ylabel);

	Expression textScale = Expression(1);

	for (auto z : options.getTail()) {
		if (z.getTail().at(0).head() == Atom("\"text-scale\"")) {
			textScale = z.getTail().at(1);
		}
	}

	for (auto o : options.getTail()) {
		Expression text = o.getTail().at(1);
		text.setProperty("\"object-name\"", TEXT);

		if (o.getTail().at(0).head() == Atom("\"title\"")) {
			text.setProperty("\"position\"", titlepositionexp);
			text.setProperty("\"text-scale\"", textScale);
			text.setProperty("\"text-rotation\"", Expression(0));
			ret.push_back(text);
		}
		else if (o.getTail().at(0).head() == Atom("\"abscissa-label\"")) {
			text.setProperty("\"position\"", xlabelexp);
			text.setProperty("\"text-scale\"", textScale);
			text.setProperty("\"text-rotation\"", Expression(0));
			ret.push_back(text);
		}
		else if (o.getTail().at(0).head() == Atom("\"ordinate-label\"")) {
			text.setProperty("\"position\"", ylabelexp);
			Expression rotate = Expression(std::atan2(0, -1) / 2);
			text.setProperty("\"text-rotation\"", rotate);
			text.setProperty("\"text-scale\"", textScale);
			ret.push_back(text);
		}
	}

	Expression xminlabelpos(Atom("list"));
	xminlabelpos.append(Expression(x_min*xscale));
	xminlabelpos.append(Expression(-y_min*yscale + 2));

	std::stringstream xmin;
	xmin << "\"" << std::setprecision(2) << x_min << "\"";
	Expression xminlabel = Expression(Atom(xmin.str()));
	xminlabel.setProperty("\"object-name\"", TEXT);
	xminlabel.setProperty("\"position\"", xminlabelpos);
	xminlabel.setProperty("\"text-scale\"", textScale);
	xminlabel.setProperty("\"text-rotation\"", Expression(0));
	ret.push_back(xminlabel);

	Expression xmaxlabelpos(Atom("list"));
	xmaxlabelpos.append(Expression(x_max*xscale));
	xmaxlabelpos.append(Expression(-y_min*yscale + 2));

	std::stringstream xmax;
	xmax << "\"" << std::setprecision(2) << x_max << "\"";
	Expression xmaxlabel = Expression(Atom(xmax.str()));
	xmaxlabel.setProperty("\"object-name\"", TEXT);
	xmaxlabel.setProperty("\"position\"", xmaxlabelpos);
	xmaxlabel.setProperty("\"text-scale\"", textScale);
	xmaxlabel.setProperty("\"text-rotation\"", Expression(0));
	ret.push_back(xmaxlabel);

	Expression yminlabelpos(Atom("list"));
	yminlabelpos.append(Expression(x_min*xscale - 2));
	yminlabelpos.append(Expression(-y_min*yscale));

	std::stringstream ymin;
	ymin << "\"" << std::setprecision(2) << y_min << "\"";
	Expression yminlabel = Expression(Atom(ymin.str()));
	yminlabel.setProperty("\"object-name\"", TEXT);
	yminlabel.setProperty("\"position\"", yminlabelpos);
	yminlabel.setProperty("\"text-scale\"", textScale);
	yminlabel.setProperty("\"text-rotation\"", Expression(0));
	ret.push_back(yminlabel);

	Expression ymaxlabelpos(Atom("list"));
	ymaxlabelpos.append(Expression((x_min*xscale) - 2));
	ymaxlabelpos.append(Expression(-y_max*yscale));

	std::stringstream ymax;
	ymax << "\"" << std::setprecision(2) << y_max << "\"";
	Expression ymaxlabel = Expression(Atom(ymax.str()));
	ymaxlabel.setProperty("\"object-name\"", TEXT);
	ymaxlabel.setProperty("\"position\"", ymaxlabelpos);
	ymaxlabel.setProperty("\"text-scale\"", textScale);
	ymaxlabel.setProperty("\"text-rotation\"", Expression(0));
	ret.push_back(ymaxlabel);


	return Expression(ret);
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
				for (size_t i = 0; i < params.size(); i++) {
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
			//If the first arg is a procedure (not lambda)
			if (env.is_proc(args[0].head())) {
				//create a return list of values as answer
					Expression ret(Atom("list"));
					//for each input parameter
					for (auto a : args[1].getTail()) {
						Expression val(args[0].head());
						//if its a list, throw an error
						if (a.head() == Atom("list")) {
							throw SemanticError("Error in call to map, invalid list argument");
						}
						//not list, just grab the number
						else {
							val.append(a.head());
						}
						//evaluate and add to list of answers
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
			else if (args[0].isHeadLambda()) {
				//create new environment
				Environment newEnv = Environment(env);
				//get the list of parameter symbols
				std::vector<Expression> params = args.at(0).getTail().at(0).getTail();
				std::vector<Expression> inputs = args[1].getTail();


				std::vector<Expression> ret;
				Expression val;

				if (params.size() != 1) {
					throw SemanticError("Error in call to map, cannot map to lambda with multiple inputs");
				}

				for (auto a : inputs) {
					//save the inputs as known expressions
					for (size_t i = 0; i < params.size(); i++) {
						newEnv.add_exp(params[i].head(), a, true);
					}

					//evaluate with that input
					val = args.at(0).getTail().at(1).eval(newEnv);
					ret.push_back(val);
				}
				return Expression(ret);

			}
			else {
				throw SemanticError("Error in call to map: first arg must be a procedure or lambda function");
			}
		}
		else {
			throw SemanticError("Error in call to map: second arg needs to be list");
		}
	}
	else {
		throw SemanticError("Error in call to map: invalid number of arguments.");
	}

};

//Function that adds a the second Expression with the key being the first Expression (if string) 
//to the third arguments property-list
Expression set_property(std::vector<Expression> & args) {
	if (nargs_equal(args, 3)) {
		if (args[0].isHeadString()) {
			args.at(2).setProperty(args.at(0).head().asString(), args.at(1));
		}
		else {
			throw SemanticError("Error in call to set-property, first argument not a string");
		}
	}
	else {
		throw SemanticError("Error in call to set-property, need 3 arguments");
	}

	return args[2];
};

//Returns the property given the first arg is a string key and second arg has that property
Expression get_property(std::vector<Expression> & args) {
	if (nargs_equal(args, 2)) {
		if (args[0].isHeadString()) {
				return args.at(1).getProperty(args.at(0).head().asString());
		}
		else {
			throw SemanticError("Error in call to get-property, first argument not a string");
		}
	}
	else {
		throw SemanticError("Error in call to get-property, need 2 arguments");
	}


};



/////////////////////////////////////////End of defined procedures

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
    //throw SemanticError("Attempt to overwrite symbol in environemnt");
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

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if ((result != envmap.end()) && (result->second.type == ProcedureBiType)) {
			return result->second.proc_bi;
		}
	}

	return default_proc_bi;
}

bool Environment::is_proc_prop(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ProcedurePropType);
}

Procedure_prop Environment::get_proc_prop(const Atom &sym) const {

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if ((result != envmap.end()) && (result->second.type == ProcedurePropType)) {
			return result->second.proc_prop;
		}
	}

	return default_proc_prop;
}

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> I(0.0, 1.0);

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset() {

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

	// Procedure: discrete-plot;
	envmap.emplace("discrete-plot", EnvResult(ProcedurePropType, discrete_plot));

	// Binary Procedure: apply;
	envmap.emplace("apply", EnvResult(ProcedureBiType, apply));

	// Binary Procedure: map;
	envmap.emplace("map", EnvResult(ProcedureBiType, map));

	// Binary Procedure: set-property;
	envmap.emplace("set-property", EnvResult(ProcedurePropType, set_property));

	// Binary Procedure: get-property;
	envmap.emplace("get-property", EnvResult(ProcedurePropType, get_property));
}
