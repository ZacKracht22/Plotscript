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

//Helper function that returns an expression containing a line made from point1 to point2 using the scaling values
Expression makeLineFromPoints(const Expression& point1, const Expression& point2, double xscale, double yscale) {
	const Expression THICKNESS(0);
	const Expression LINE(Atom("\"line\""));

	double point1x = point1.getTail().at(0).head().asNumber()*xscale;
	double point1y = -point1.getTail().at(1).head().asNumber()*yscale;
	double point2x = point2.getTail().at(0).head().asNumber()*xscale;
	double point2y = -point2.getTail().at(1).head().asNumber()*yscale;

	std::vector<Expression> make_line;
	std::vector<Expression> point1Vec;
	point1Vec.push_back(Expression(point1x));
	point1Vec.push_back(Expression(point1y));
	std::vector<Expression> point2Vec;
	point2Vec.push_back(Expression(point2x));
	point2Vec.push_back(Expression(point2y));
	make_line.push_back(point1Vec);
	make_line.push_back(point2Vec);
	Expression line = Expression(make_line);
	line.setProperty("\"object-name\"", LINE);
	line.setProperty("\"thickness\"", THICKNESS);

	return line;
};

//Function that takes in the return vector and min's, max's, and scales to add the graph's border lines to it
void makeGraphBorder(std::vector<Expression>& ret, double x_min, double x_max, double y_min, double y_max, double xscale, double yscale) {
	//Add lines for the boundaries of the graph
	std::vector<Expression> bottom_line_left;
	std::vector<Expression> bottom_line_right;
	bottom_line_left.push_back(Expression(x_min*xscale));
	bottom_line_left.push_back(Expression(y_min*yscale));
	bottom_line_right.push_back(Expression((x_min*xscale) + 20));
	bottom_line_right.push_back(Expression(y_min*yscale));
	Expression bottom_line = makeLineFromPoints(Expression(bottom_line_left), Expression(bottom_line_right), 1, 1);
	ret.push_back(bottom_line);

	std::vector<Expression> top_line_left;
	std::vector<Expression> top_line_right;
	top_line_left.push_back(Expression(x_min*xscale));
	top_line_left.push_back(Expression(y_max*yscale));
	top_line_right.push_back(Expression((x_min*xscale) + 20));
	top_line_right.push_back(Expression(y_max*yscale));
	Expression top_line = makeLineFromPoints(Expression(top_line_left), Expression(top_line_right), 1, 1);
	ret.push_back(top_line);

	std::vector<Expression> left_line_bottom;
	std::vector<Expression> left_line_top;
	left_line_bottom.push_back(Expression(x_min*xscale));
	left_line_bottom.push_back(Expression(y_min*yscale));
	left_line_top.push_back(Expression(x_min*xscale));
	left_line_top.push_back(Expression((y_min*yscale) + 20));
	Expression left_line = makeLineFromPoints(Expression(left_line_bottom), Expression(left_line_top), 1, 1);
	ret.push_back(left_line);

	std::vector<Expression> right_line_bottom;
	std::vector<Expression> right_line_top;
	right_line_bottom.push_back(Expression(x_max*xscale));
	right_line_bottom.push_back(Expression(y_min*yscale));
	right_line_top.push_back(Expression(x_max*xscale));
	right_line_top.push_back(Expression((y_min*yscale) + 20));
	Expression right_line = makeLineFromPoints(Expression(right_line_bottom), Expression(right_line_top), 1, 1);
	ret.push_back(right_line);

	//Add axis lines if they are in the range of the graph
	if (x_min < 0 && x_max > 0) {
		std::vector<Expression> middle_vertical_line_bottom;
		std::vector<Expression> middle_vertical_line_top;
		middle_vertical_line_bottom.push_back(Expression(0));
		middle_vertical_line_bottom.push_back(Expression(y_min*yscale));
		middle_vertical_line_top.push_back(Expression(0));
		middle_vertical_line_top.push_back(Expression(y_max*yscale));
		Expression middle_vertical_line = makeLineFromPoints(Expression(middle_vertical_line_bottom), Expression(middle_vertical_line_top), 1, 1);
		ret.push_back(middle_vertical_line);
	}

	if (y_min < 0 && y_max > 0) {
		std::vector<Expression> middle_horizontal_line_left;
		std::vector<Expression> middle_horizontal_line_right;
		middle_horizontal_line_left.push_back(Expression(x_min*xscale));
		middle_horizontal_line_left.push_back(Expression(0));
		middle_horizontal_line_right.push_back(Expression(x_max*xscale));
		middle_horizontal_line_right.push_back(Expression(0));
		Expression middle_horizontal_line = makeLineFromPoints(Expression(middle_horizontal_line_left), Expression(middle_horizontal_line_right), 1, 1);
		ret.push_back(middle_horizontal_line);
	}

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

	//Calculate x and y bounds
	for (auto d : data.getTail()) {
		double x_point = d.getTail().at(0).head().asNumber();
		double y_point = d.getTail().at(1).head().asNumber();

		if (x_point > x_max) x_max = x_point;
		if (x_point < x_min) x_min = x_point;
		if (y_point > y_max) y_max = y_point;
		if (y_point < y_min) y_min = y_point;
	}

	//Calculate x and y scales and mids
	const double xscale = 20 / (x_max - x_min);
	const double yscale = 20 / (y_max - y_min);
	double xmiddle = (x_max + x_min) / 2;
	double ymiddle = (y_max + y_min) / 2;

	//Add each point from the data list
	for (auto a : data.getTail()) {
		std::vector<Expression> make_point;
		make_point.push_back(Expression(a.getTail().at(0).head().asNumber() * xscale));
		make_point.push_back(Expression(-a.getTail().at(1).head().asNumber() * yscale));
		Expression point = Expression(make_point);
		point.setProperty("\"object-name\"", POINT);
		point.setProperty("\"size\"",SIZE);
		points.push_back(point);

		std::vector<Expression> make_axis_point;
		Expression axis_point;

		//Draw line to min axis if  y = 0 is not present
		if (y_min > 0) {
			make_axis_point.push_back(Expression(a.getTail().at(0).head().asNumber() * xscale));
			make_axis_point.push_back(Expression(-y_min*yscale));
			axis_point = Expression(make_axis_point);
		}
		//Draw line to max axis if  y = 0 is not present
		else if (y_max < 0) {
			make_axis_point.push_back(Expression(a.getTail().at(0).head().asNumber() * xscale));
			make_axis_point.push_back(Expression(-y_max*yscale));
			axis_point = Expression(make_axis_point);
		}
		//Draw line to y axis if present
		else{
			make_axis_point.push_back(Expression(a.getTail().at(0).head().asNumber() * xscale));
			make_axis_point.push_back(Expression(0));
			axis_point = Expression(make_axis_point);
		}

		std::vector<Expression> make_line;
		make_line.push_back(point);
		make_line.push_back(axis_point);
		Expression line = Expression(make_line);
		line.setProperty("\"object-name\"", LINE);
		line.setProperty("\"thickness\"", THICKNESS);
		ret.push_back(line);
	}

	//Make the lines for the graph border
	makeGraphBorder(ret, x_min, x_max, y_min, y_max, xscale, yscale);

	//Add all the points to the return vector
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
	//Check if a new scale is given
	for (auto z : options.getTail()) {
		if (z.getTail().at(0).head() == Atom("\"text-scale\"")) {
			textScale = z.getTail().at(1);
		}
	}

	//Find the options
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


//Algorithm to split lines who's angle is less than 175 degrees to smooth the plot
void recursiveLineSplitAlgorithm(std::vector<Expression>& ret, double xscale, double yscale, std::size_t z /*index*/) {
	float point1x = ret.at(z).getTail().at(0).getTail().at(0).head().asNumber() / xscale;
	float point1y = -ret.at(z).getTail().at(0).getTail().at(1).head().asNumber() / yscale;
	float point2x = ret.at(z).getTail().at(1).getTail().at(0).head().asNumber() / xscale;
	float point2y = -ret.at(z).getTail().at(1).getTail().at(1).head().asNumber() / yscale;

	float point3x = ret.at(z + 1).getTail().at(0).getTail().at(0).head().asNumber() / xscale;
	float point3y = -ret.at(z + 1).getTail().at(0).getTail().at(1).head().asNumber() / yscale;
	float point4x = ret.at(z + 1).getTail().at(1).getTail().at(0).head().asNumber() / xscale;
	float point4y = -ret.at(z + 1).getTail().at(1).getTail().at(1).head().asNumber() / yscale;

	double slope1 = (point2y - point1y) / (point2x - point1x);
	double slope2 = (point4y - point3y) / (point4x - point3x);

	//Calculate the angle from the two slopes
	double angle = (std::atan(slope1) - std::atan(slope2)) * 180 / std::atan2(0, -1);

	//Update the angle if the wrong angle is found
	if (abs(angle) < 90) {
		angle = 180 - abs(angle);
	}
	
	if (angle < 175) {
		//Calculate 3 new lines
		Expression newPoint1X = Expression(point1x);
		Expression newPoint1Y = Expression(point1y);
		std::vector<Expression> newPoint1Vec;
		newPoint1Vec.push_back(newPoint1X);
		newPoint1Vec.push_back(newPoint1Y);
		Expression newPoint1 = Expression(newPoint1Vec);

		Expression newPoint2X = Expression((point2x + point1x) / 2.000);
		Expression newPoint2Y = Expression((point2y + point1y) / 2.000);
		std::vector<Expression> newPoint2Vec;
		newPoint2Vec.push_back(newPoint2X);
		newPoint2Vec.push_back(newPoint2Y);
		Expression newPoint2 = Expression(newPoint2Vec);

		Expression newPoint3X = Expression((point4x + point3x) / 2.000);
		Expression newPoint3Y = Expression((point4y + point3y) / 2.000);
		std::vector<Expression> newPoint3Vec;
		newPoint3Vec.push_back(newPoint3X);
		newPoint3Vec.push_back(newPoint3Y);
		Expression newPoint3 = Expression(newPoint3Vec);

		Expression newPoint4X = Expression(point4x);
		Expression newPoint4Y = Expression(point4y);
		std::vector<Expression> newPoint4Vec;
		newPoint4Vec.push_back(newPoint4X);
		newPoint4Vec.push_back(newPoint4Y);
		Expression newPoint4 = Expression(newPoint4Vec);

		//Make new lines
		Expression newLineLeft = makeLineFromPoints(newPoint1, newPoint2, xscale, yscale);

		Expression newLineMiddle = makeLineFromPoints(newPoint2, newPoint3, xscale, yscale);

		Expression newLineRight = makeLineFromPoints(newPoint3, newPoint4, xscale, yscale);

		//Erase the old lines and replace with new split slines
		ret.erase(ret.begin() + z);
		ret.erase(ret.begin() + z);
		ret.insert(ret.begin() + z, newLineLeft);
		ret.insert(ret.begin() + z + 1, newLineMiddle);
		ret.insert(ret.begin() + z + 2, newLineRight);
	}

};


//Function returns a list of lines and texts to create a continuous plot
Expression continuous_plot(const std::vector<Expression> & args, Environment & env) {
	const Expression TEXT(Atom("\"text\""));

	Expression func = args.at(0);
	Expression bounds = args.at(1);


	std::vector<Expression> ret;
	std::vector<Expression> points;

	//Get the x bounds from second list and calculate the scale/middle
	double x_min = bounds.getTail().at(0).head().asNumber();
	double x_max = bounds.getTail().at(1).head().asNumber();
	double pointSpacing = (x_max - x_min) / 50;

	double xscale = 20 / (x_max - x_min);
	double xmiddle = (x_max + x_min) / 2;

	Expression lambdaVariable = func.getTail().at(0).getTail().at(0);

	Expression lambdaFunc = func.getTail().at(1);

	//Begin by adding 51 points to a vector of points
	for (double i = x_min; i <= (x_max + pointSpacing); i += pointSpacing) {
	
		Environment temp(env);
		temp.add_exp(lambdaVariable.head(), Expression(i), true);

		Expression tempExp = Expression(args.at(0).getTail()).eval(temp);
		Expression y_point = tempExp.getTail().at(1);

		std::vector<Expression> point = {Expression(i),y_point};
		points.push_back(Expression(point));
	}

	double y_min = 1000000;
	double y_max = -1000000;

	//Use the present points to determine the y bounds, then calculate scale and middle
	for (auto a : points) {
		double y = a.getTail().at(1).head().asNumber();
		if (y > y_max) y_max = y;
		if (y < y_min) y_min = y;
	}

	double yscale = 20 / (y_max - y_min);
	double ymiddle = (y_max + y_min) / 2;

	//Make 50 lines fromm the initial 51 points
	for (std::size_t index = 0; index < points.size() - 1; index++) {
		Expression line = makeLineFromPoints(points[index], points[index + 1], xscale, yscale);
		ret.push_back(line);
	}

	// Run line split checking 10 times
	for (int max10 = 0; max10 < 10; max10++) {
		std::size_t size = ret.size();
		//Run through graph searching for angles <175 in algorithm
		for (std::size_t z = 0; z < size - 1; z++) {
			recursiveLineSplitAlgorithm(ret, xscale, yscale, z);
		}
	}
		

	//Make graph's border points
	makeGraphBorder(ret, x_min, x_max, y_min, y_max, xscale, yscale);

	//Add the necessary labels
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

	if (args.size() == 3) {
		Expression options = args.at(2);

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

	// Procedure: continuous-plot;
	envmap.emplace("continuous-plot", EnvResult(ProcedureBiType, continuous_plot));

	// Binary Procedure: apply;
	envmap.emplace("apply", EnvResult(ProcedureBiType, apply));

	// Binary Procedure: map;
	envmap.emplace("map", EnvResult(ProcedureBiType, map));

	// Binary Procedure: set-property;
	envmap.emplace("set-property", EnvResult(ProcedurePropType, set_property));

	// Binary Procedure: get-property;
	envmap.emplace("get-property", EnvResult(ProcedurePropType, get_property));
}
