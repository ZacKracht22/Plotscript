#include "expression.hpp"

#include <sstream>
#include <list>
#include <iostream>

#include "environment.hpp"
#include "semantic_error.hpp"

Expression::Expression(){}

Expression::Expression(const Atom & a){

  m_head = a;
}

Expression::Expression(const std::vector<Expression> & args) {
	m_head = Atom("list");
	m_tail = args;
}

Expression::Expression(const Expression & tail0, const Expression & tail1) {
	std::vector<Expression> tail;
	tail.push_back(tail0);
	tail.push_back(tail1);

	m_head = Atom("lambda");
	m_tail = tail;
}

// recursive copy
Expression::Expression(const Expression & a){

  m_head = a.m_head;
  property_list = a.property_list;
  for(auto e : a.m_tail){
    m_tail.push_back(e);
  }
}

Expression & Expression::operator=(const Expression & a){

  // prevent self-assignment
  if(this != &a){
    m_head = a.m_head;
	property_list = a.property_list;
    m_tail.clear();
    for(auto e : a.m_tail){
      m_tail.push_back(e);
    } 
  }
  
  return *this;
}


Atom & Expression::head(){
  return m_head;
}

Atom Expression::getHead() {
	return m_head;
}

const Atom & Expression::head() const{
  return m_head;
}

bool Expression::isHeadNumber() const noexcept{
  return m_head.isNumber();
}

bool Expression::isHeadSymbol() const noexcept{
  return m_head.isSymbol();
}  

bool Expression::isHeadComplex() const noexcept {
	return m_head.isComplex();
}

bool Expression::isHeadString() const noexcept {
	return m_head.isString();
}

bool Expression::isHeadNone() const noexcept {
	return m_head.isNone();
}

bool Expression::isHeadLambda() const noexcept {
	return m_head == Atom("lambda");
}

bool Expression::isHeadList() const noexcept {
	return m_head == Atom("list");
}

void Expression::append(const Atom & a){
  m_tail.emplace_back(a);
}


Expression * Expression::tail(){
  Expression * ptr = nullptr;
  
  if(m_tail.size() > 0){
    ptr = &m_tail.back();
  }

  return ptr;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept{
  return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept{
  return m_tail.cend();
}


Expression apply(const Atom & op, const std::vector<Expression> & args,  Environment & env){

  // head must be a symbol
  if(!op.isSymbol()){
    throw SemanticError("Error during evaluation: procedure name not symbol");
  }

  if (env.is_exp(op)) {
	  //get the lambda expression
	  Expression lambdaExp = env.get_exp(op);
	  //create new environment
	  Environment newEnv = Environment(env);

	  //get the list of parameter symbols
	  std::vector<Expression> params = lambdaExp.getTail().at(0).getTail();

	  //if the size of the arguments and the size of the inputs dont match throw an error
	  if (params.size() != args.size()) {
		  throw SemanticError("Error during evaluation: lambda function called with incorrect number of args");
	  }


	  //save the inputs as known expressions
	  for (size_t i = 0; i < params.size(); i++) {
		  newEnv.add_exp(params[i].head(), args[i], true);
	  }

	  //return the evaluation
	  return lambdaExp.getTail().at(1).eval(newEnv);
  }
  else if (env.is_proc(op)) {
	  // map from symbol to proc
	  Procedure proc = env.get_proc(op);
	  // call proc with args
	  return proc(args);
  }
  else if (env.is_proc_bi(op)){
		  // map from symbol to binary proc
		  Procedure_bi proc_bi = env.get_proc_bi(op);
		  // call proc_bi with args
		  return proc_bi(args, env);
  }
  else if (env.is_proc_prop(op)) {
	  // map from symbol to property proc
	  Procedure_prop proc_prop = env.get_proc_prop(op);
	  // call proc_bi with args
	  std::vector<Expression> nonConstArgs = args;
	  return proc_prop(nonConstArgs);
  }
  else
	  throw SemanticError("Error during evaluation: symbol does not name a procedure");

}

Expression Expression::handle_lookup(const Atom & head, const Environment & env){
    if(head.isSymbol())
	{ // if symbol is in env return value
      if(env.is_exp(head))
	  {
		return env.get_exp(head);
      }
	  else if (env.is_proc(head) || env.is_proc_bi(head) || env.is_proc_prop(head)) {
		  return Expression(head);
	  }
      else
	  {
		throw SemanticError("Error during evaluation: unknown symbol");
      }
    }
    else if(head.isNumber() || head.isComplex() || head.isString()){
      return Expression(head);
    }
    else{
      throw SemanticError("Error during evaluation: Invalid type in terminal expression");
    }
}

Expression Expression::handle_begin(Environment & env){
  
  if(m_tail.size() == 0){
    throw SemanticError("Error during evaluation: zero arguments to begin");
  }

  // evaluate each arg from tail, return the last
  Expression result;
  for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
    result = it->eval(env);
  }
  
  return result;
}


Expression Expression::handle_define(Environment & env) {

	// tail must have size 3 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to define");
	}

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}

	// but tail[0] must not be a special-form or procedure
	std::string s = m_tail[0].head().asSymbol();
	if ((s == "define") || (s == "begin")) {
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");
	}

	if (env.is_proc(m_head) || env.is_proc_bi(m_head) || env.is_proc_prop(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}

	// eval tail[1]
	Expression result = m_tail[1].eval(env);

	/*if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}*/

	//and add to env
	env.add_exp(m_tail[0].head(), result, false);

	return result;
}


Expression Expression::handle_lambda() {

	// tail must have size 2 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to define");
	}

	//create a vector of parameters as expressions
	std::vector<Expression> parameters;
	//push back the head of the first tail member
	parameters.push_back(m_tail[0].head());

	//for each member of the tail of the first tail member, add to the list of parameters
	for (auto e = m_tail[0].tailConstBegin(); e != m_tail[0].tailConstEnd(); ++e) {
		if (e->isHeadSymbol()) {
			parameters.push_back(*e);
		}
	}

	Expression retExpTail0(parameters);
	Expression retExpTail1 = m_tail[1];

	return Expression(retExpTail0, retExpTail1);
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env) {

	if (m_tail.empty() && m_head != Atom("list")) {
		return handle_lookup(m_head, env);
	}
	// handle begin special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "begin") {
		return handle_begin(env);
	}
	// handle define special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "define") {
		return handle_define(env);
	}
	// handle lambda special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "lambda") {
		return handle_lambda();
	}
	// else attempt to treat as procedure
	else {
		std::vector<Expression> results;
		for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
			results.push_back(it->eval(env));
		}
		return apply(m_head, results, env);
	}
}


std::ostream & operator<<(std::ostream & out, const Expression & exp) {

	if (exp == Expression()) {
		out << "NONE";
		return out;
	}

	if (!exp.head().isComplex())
	{
		out << "(";
		if (exp.head() != Atom("list") && exp.head() != Atom("lambda")) {
			out << exp.head();
			if (exp.tailLength() > 0) {
				out << " ";
			}
		}

		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
			if(e == exp.tailConstBegin()){
				out << *e;
			}
			else {
				out << " " << *e;
			}
		}

		out << ")";
	}
	else
	{
		out << exp.head();

		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
			out << *e;
		}
	}
	

  return out;
}

bool Expression::operator==(const Expression & exp) const noexcept{

  bool result = (m_head == exp.m_head);

  result = result && (m_tail.size() == exp.m_tail.size());

  if(result){
    for(auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
	(lefte != m_tail.end()) && (righte != exp.m_tail.end());
	++lefte, ++righte){
      result = result && (*lefte == *righte);
    }
  }

  return result;
}

bool operator!=(const Expression & left, const Expression & right) noexcept{

  return !(left == right);
}

bool operator<(const Expression & left, const Expression & right) noexcept {

	return !(left == right);
}

//Returns the tail of an expression
std::vector<Expression> Expression::getTail() const noexcept {
	return m_tail;
}

//Returns how many expressions are in the tail of an expression
size_t Expression::tailLength() const noexcept {
	return m_tail.size();
}

Expression Expression::getProperty(std::string key){
	return property_list[key];
}


void Expression::setProperty(std::string key, Expression& val){
	property_list[key] = val;
}

void Expression::setPropertyList(std::map<std::string, Expression> map) {
	property_list = map;
}

std::map<std::string, Expression> Expression::getPropertyList() {
	return property_list;
}

std::string expString(Expression& exp) {
	std::stringstream out;

	if (exp == Expression()) {
		out << "NONE";
		return out.str();
	}

	if (!exp.head().isComplex())
	{
		out << "(";
		if (exp.head() != Atom("list") && exp.head() != Atom("lambda")) {
			out << exp.head();
			if (exp.tailLength() > 0) {
				out << " ";
			}
		}

		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
			if (e == exp.tailConstBegin()) {
				out << *e;
			}
			else {
				out << " " << *e;
			}
		}

		out << ")";
	}
	else
	{
		out << exp.head();

		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
			out << *e;
		}
	}


	return out.str();
}

