#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <complex>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){
  
  std::istringstream iss(program);
    
  Interpreter interp;
    
  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl; 
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);
 
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};
  
  for(auto program : programs){
    std::istringstream iss(program);
 
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");
    
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {

  {
    std::string program = "(f";
    std::istringstream iss(program);
  
    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
  
  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {

  std::string program = "hello";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {

  std::string program;
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {

  std::string program = "( )";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {

  std::string program = "(1abc)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {
  
  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

}

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }
  
  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }
}
  
TEST_CASE( "Test Interpreter special forms: begin and define", "[interpreter]" ) {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }
  
  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE( "Test a medium-sized expression", "[interpreter]" ) {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(43.));
  }
}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
					 "(+ -3 1 1)",
					 "(- 1)",
					 "(- 1 2)",
					 "(* 1 -1)",
					 "(* 1 1 -1)",
					 "(/ -1 1)",
					 "(/ 1 -1)"};

    for(auto s : programs){
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}


TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {
  
	std::vector<std::string> programs = { "(@ none)", // no such procedure
						 "(- 1 1 2)", // too many arguments
						 "(define begin 1)", // redefine special form
						 "(+ 1 2 3 @)", //can't add a symbol
						 "(* 1 2 3 @)", //can't multiply a symbol
						 "(- 1 2 3)", //can't subtract more than 2 numbers
						 "(- @)", //can't negate a symbol
						 "(- 1 @)", //can't subtract a symbol
						 "(/ 1 2 3 4)", //can't divide more than 2 numbers
						 "(/ 1 !)", //can't divide a symbol
						 "(sqrt 2 3)", //can't call sqrt with more than 1 arg
						 "(pow 1 2 3)", //can't call pow with more than 2 args
						 "(pow 1 !)",
						 "(pow 1)",
						 "(ln (- 5))", //can't call ln with a negative number
						 "(ln 1 2)",
						 "(sin I)", //can't call sin with complex num
						 "(cos I)", //can't call cos with complex num
						 "(tan I)", //can't call tan with complex num
						 "(sin 1 2)", //can't call sin with multiple args
						 "(cos 1 2)", //can't call cos with multiple args
						 "(tan 1 2)", //can't call tan with multiple args
						  "(real 2)", //can't call real with number
						  "(mag 5)",  //cant call mag with number
						  "(imag 8)", //can't call imag with number
						  "(real I I I)", //can't call real with 3 args
						  "(mag I I I)",  //cant call mag with 3 args
						  "(imag I I I)",//cant call imag with 3 args
						  "(arg 3)", //cant call arg with a number
						  "(arg I I I)", //cant call arg with multiple arguments
						  "(conj 3)", //cant call conj with a number
		"(begin)",
		"(define 1 1)",
		"(lambda (x y))",
		"(begin (define f lambda (x) (x)) (f 3 3))",
		"(begin)",
						  "(conj I I I)" };//cant call conj with multiple arguments
    for(auto s : programs){
      Interpreter interp;
	  INFO(s);
      std::istringstream iss(s);
      
      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);
      
      REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
    }

}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {

  std::string input = R"(
(+ 1 a)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {

    std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
    std::string input = R"(
(1 2 3)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Sqrt tests", "[interpreter]") {

	{
		std::string program = "(sqrt 9)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(3.));
	}

	{
		std::string program = "(sqrt 36)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(6.));
	}

	{
		std::string program = "(sqrt (- 1))";
		INFO(program);
		Expression result = run(program);
		std::complex<double> expected(0.0,1.0);
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(sqrt I)";
		INFO(program);
		Expression result = run(program);
		std::complex<double> expected = sqrt(std::complex<double>(0.0,1.0));
		REQUIRE(result == Expression(expected));
	}

}

TEST_CASE("Pow tests", "[interpreter]") {

	{
		std::string program = "(^ 3 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(9.));
	}

	{
		std::string program = "(^ 9 0.5)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(3.));
	}

	{
		std::string program = "(^ 5 0)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1.));
	}

	{
		std::string program = "(^ 2 -2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0.25));
	}

	{
		std::string program = "(^ I I)";
		INFO(program);
		Expression result = run(program);
		std::complex<double> I(0.0,1.0);
		std::complex<double> expected = std::pow(I,I);
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(^ 1 I)";
		INFO(program);
		Expression result = run(program);
		std::complex<double> expected(1.0,0.0);
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(^ I 2)";
		INFO(program);
		Expression result = run(program);
		std::complex<double> I(0.0, 1.0);
		std::complex<double> expected = std::pow(I, 2.0);
		REQUIRE(result == Expression(expected));
	}

}


TEST_CASE("ln tests", "[interpreter]") {

	{
		std::string program = "(ln 1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0.));
	}

	{
		std::string program = "(ln 5)";
		INFO(program);
		Expression result = run(program);
		double expected = log(5);
		REQUIRE(result == Expression(expected));
	}

}

TEST_CASE("sin tests", "[interpreter]") {

	{
		std::string program = "(sin 0)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0.));
	}

	{
		std::string program = "(sin (/ pi 2))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1.));
	}

}

TEST_CASE("cos tests", "[interpreter]") {

	{
		std::string program = "(cos 0)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1.));
	}

	{
		std::string program = "(cos (/ pi 2))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0.));
	}

}

TEST_CASE("tan tests", "[interpreter]") {

	{
		std::string program = "(tan (/ pi 4))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1.));
	}

	{
		std::string program = "(tan 0)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0.));
	}

}

TEST_CASE("complex add tests", "[interpreter]") {

	{
		std::string program = "(+ I I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(0.0,2.0)));
	}

	{
		std::string program = "(+ I I I I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(0.0, 4.0)));
	}

	{
		std::string program = "(+ I 1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(1.0, 1.0)));
	}

	{
		std::string program = "(+ 2 I 1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(3.0, 1.0)));
	}
}

TEST_CASE("complex sub/neg tests", "[interpreter]") {

	{
		std::string program = "(- I I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(0.0, 0.0)));
	}

	{
		std::string program = "(- I 1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(-1.0, 1.0)));
	}

	{
		std::string program = "(- 1 I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(1.0, -1.0)));
	}

	{
		std::string program = "(- I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(0.0, -1.0)));
	}

}

TEST_CASE("complex multiply tests", "[interpreter]") {

	{
		std::string program = "(* I I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(-1.0, 0.0)));
	}

	{
		std::string program = "(* I 0)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(0.0, 0.0)));
	}

	{
		std::string program = "(* I 1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(0.0, 1.0)));
	}

	{
		std::string program = "(* 1 I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(0.0, 1.0)));
	}

	{
		std::string program = "(* I I I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(0.0, -1.0)));
	}


}

TEST_CASE("complex division tests", "[interpreter]") {

	{
		std::string program = "(/ I I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(1.0, 0.0)));
	}

	{
		std::string program = "(/ I 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(0.0, 0.5)));
	}

	{
		std::string program = "(/ 2 I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(0.0, -2.0)));
	}

	{
		std::string program = "(/ 2 1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(2.));
	}

	{
		std::string program = "(/ 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0.5));
	}

	{
		std::string program = "(/ 4)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0.25));
	}

	{
		std::string program = "(/ I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::complex<double>(0.0,-1.0)));
	}

}

TEST_CASE("complex real tests", "[interpreter]") {

	{
		std::string program = "(real I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0.));
	}

	{
		std::string program = "(real (+ I 3))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(3.));
	}

	{
		std::string program = "(real (- I 3))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(-3.));
	}

}

TEST_CASE("complex imag tests", "[interpreter]") {

	{
		std::string program = "(imag I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1.));
	}

	{
		std::string program = "(imag (+ I I))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(2.));
	}

	{
		std::string program = "(imag (- (- I I) I))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(-1.));
	}

}

TEST_CASE("complex mag tests", "[interpreter]") {

	{
		std::string program = "(mag I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1.));
	}

	{
		std::string program = "(mag (+ 1 I))";
		INFO(program);
		Expression result = run(program);
		double expected = sqrt(2);
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(mag (+ 2 I))";
		INFO(program);
		Expression result = run(program);
		double expected = sqrt(5);
		REQUIRE(result == Expression(expected));
	}
	{
		std::string program = "(mag (+ I I))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(2.));
	}
}

TEST_CASE("complex arg tests", "[interpreter]") {

	{
		std::string program = "(arg I)";
		INFO(program);
		Expression result = run(program);
		double expected = std::atan2(0, -1) / 2;
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(arg (+ 1 I))";
		INFO(program);
		Expression result = run(program);
		double expected = std::atan2(0, -1) / 4;
		REQUIRE(result == Expression(expected));
	}

}

TEST_CASE("complex conj tests", "[interpreter]") {

	{
		std::string program = "(conj I)";
		INFO(program);
		Expression result = run(program);
		std::complex<double> expected(0.0,-1.0);
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(conj (+ 1 I))";
		INFO(program);
		Expression result = run(program);
		std::complex<double> expected(1.0, -1.0);
		REQUIRE(result == Expression(expected));
	}

}

TEST_CASE("Test for creating lists using list procedure", "[interpreter]") {

	{
		//Testing the creation of an empty list
		std::string program = "(list)";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(list 1)";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(1));
		REQUIRE(result == Expression(expected));
		REQUIRE(result.isHeadList());
	}

	{
		//create a list with an int, a complex type, a list of ints, and an empty list
		std::string program = "(define mylist (list 1 (+ 1 I) (list 5 4 3 2 1) (list)))";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(1));
		expected.push_back(Expression(std::complex<double>(1.0,1.0)));
		std::vector<Expression> sublist;
		sublist.push_back(Expression(5));
		sublist.push_back(Expression(4));
		sublist.push_back(Expression(3));
		sublist.push_back(Expression(2));
		sublist.push_back(Expression(1));
		expected.push_back(Expression(sublist));
		sublist.clear();
		expected.push_back(Expression(sublist));
		REQUIRE(result == Expression(expected));
	}
}

TEST_CASE("Test for first procedure involving lists", "[interpreter]") {

	//test that all semantic errors get thrown when needed
	{
		std::vector<std::string> programs = { "(first (1))", //cannot call first on a non-list
											  "(first (list))", //cannot call first on an empty list
											  "(first (list 1 2) (list 3 4))" }; //cannot call list with multiple arguments

		for (auto s : programs) {
			Interpreter interp;
			std::istringstream iss(s);
			interp.parseStream(iss);
			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}

	//test that the first procedure works for lists of numbers
	{
		std::string program = "(first (list 2 1 8 0))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(2));
	}

	//test that the first procedure works for lists of complex
	{
		std::string program = "(first (list I 1 2 3))";
		INFO(program);
		Expression result = run(program);
		std::complex<double> expected(0.0, 1.0);
		REQUIRE(result == Expression(expected));
	}

}

TEST_CASE("Tesst for rest procedure involving lists", "[interpreter]") {

	//test that all semantic errors get thrown when needed
	{
		std::vector<std::string> programs = { "(rest (1))", //cannot call rest on a non-list
			"(rest (list))", //cannot call rest on an empty list
			"(rest (list 1 2) (list 3 4))" }; //cannot call rest with multiple arguments

		for (auto s : programs) {
			Interpreter interp;
			std::istringstream iss(s);
			interp.parseStream(iss);
			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}

	//test that the rest procedure works for lists of numbers
	{
		std::string program = "(rest (list 2 1 8 0))";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(1));
		expected.push_back(Expression(8));
		expected.push_back(Expression(0));
		REQUIRE(result == Expression(expected));
	}

	//test that the rest procedure works for lists of complex
	{
		std::string program = "(rest (list 1 I 2 I))";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(std::complex<double>(0.0,1.0)));
		expected.push_back(Expression(2));
		expected.push_back(Expression(std::complex<double>(0.0, 1.0)));
		REQUIRE(result == Expression(expected));
	}

	//test that the rest procedure works for list of 1 value
	{
		std::string program = "(rest (list 1))";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		REQUIRE(result == Expression(expected));
	}


}


TEST_CASE("Tesst for length procedure involving lists", "[interpreter]") {

	//test that all semantic errors get thrown when needed
	{
		std::vector<std::string> programs = { "(length (1))", //cannot call length on a non-list
			"(length (list 1 2) (list 3 4))" }; //cannot call length with multiple arguments

		for (auto s : programs) {
			Interpreter interp;
			std::istringstream iss(s);
			interp.parseStream(iss);
			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}

	//test that the length procedure works for lists of numbers
	{
		std::string program = "(length (list 2 1 8 0))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(4));
	}

	//test that the length procedure works for empty lists
	{
		std::string program = "(length (list))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0));
	}


}


TEST_CASE("Tesst for append procedure involving lists", "[interpreter]") {

	//test that all semantic errors get thrown when needed
	{
		std::vector<std::string> programs = { "(append 1 (list 1 2 3))", //cannot call append on a non-list first arg
			"(append (list 1 2))" }; //cannot call append with 1 argument

		for (auto s : programs) {
			Interpreter interp;
			std::istringstream iss(s);
			interp.parseStream(iss);
			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}

	{
		std::string program = "(append (list 1 2 3) 4)";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(1));
		expected.push_back(Expression(2));
		expected.push_back(Expression(3));
		expected.push_back(Expression(4));
		REQUIRE(result == Expression(expected));
	}
}

TEST_CASE("Test for join procedure involving lists", "[interpreter]") {

	//test that all semantic errors get thrown when needed
	{
		std::vector<std::string> programs = { "(join 1 (list 1 2 3))", //cannot call join on a non-list first arg
			"(join (list 1 2 3) 1)", //cannot call join on a non-list second arg
			"(join (list 1 2))" }; //cannot call join with 1 argument

		for (auto s : programs) {
			Interpreter interp;
			std::istringstream iss(s);
			interp.parseStream(iss);
			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}

	{
		std::string program = "(join (list 1 2 3) (list 4 5))";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(1));
		expected.push_back(Expression(2));
		expected.push_back(Expression(3));
		expected.push_back(Expression(4));
		expected.push_back(Expression(5));
		REQUIRE(result == Expression(expected));
	}
}

TEST_CASE("Test for range procedure involving lists", "[interpreter]") {

	//test that all semantic errors get thrown when needed
	{
		std::vector<std::string> programs = { "(range 2 1)", //cannot call range w/o 3 args
			"(range 1 I 1)", //cannot call range when all args not numbers
			"(range 1 5 (- 1))", //cannot call range with negative increment
			"(range 2 1 1)" }; //cannot call range when first arg is greater than second arg

		for (auto s : programs) {
			Interpreter interp;
			std::istringstream iss(s);
			interp.parseStream(iss);
			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}

	{
		std::string program = "(range 1 5 1)";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(1));
		expected.push_back(Expression(2));
		expected.push_back(Expression(3));
		expected.push_back(Expression(4));
		expected.push_back(Expression(5));
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(range (- 3) 1 1)";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(-3));
		expected.push_back(Expression(-2));
		expected.push_back(Expression(-1));
		expected.push_back(Expression(0));
		expected.push_back(Expression(1));
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(range 0 1 0.11)";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(0));
		expected.push_back(Expression(.11));
		expected.push_back(Expression(.22));
		expected.push_back(Expression(.33));
		expected.push_back(Expression(.44));
		expected.push_back(Expression(.55));
		expected.push_back(Expression(.66));
		expected.push_back(Expression(.77));
		expected.push_back(Expression(.88));
		expected.push_back(Expression(.99));
		REQUIRE(result == Expression(expected));
	}
}


TEST_CASE("Testa for lambda function generation", "[interpreter]") {

	{
		std::string program = R"((begin
(define addtwo (lambda (x y) (+ x y)))
((addtwo 1))
))";
		INFO(program);
		Interpreter interp;
		std::istringstream iss(program);
		interp.parseStream(iss);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	{
		std::string program = R"((begin
(define f1 (lambda (x y) (+ x y)))
(f1 1 2)
))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(3));
	
	}

	{
		std::string program = R"((begin
(define x 5)
(define f1 (lambda (x y) (+ x y)))
(f1 1 2)
))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(3));

	}

	{
		std::string program = "(define a (lambda (x y) (+ x y)))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result.isHeadLambda());

	}

}

TEST_CASE("Tests for apply function", "[interpreter]") {

	//test that all semantic errors get thrown when needed
	{
		std::vector<std::string> programs = { "(apply 2 1)",
		"(apply 1)",
		"(apply 1 (list 1 2))",
		"(+ 1 +)" };

		for (auto s : programs) {
			Interpreter interp;
			std::istringstream iss(s);
			interp.parseStream(iss);
			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}


	{
		std::string program = R"((begin
(apply + (list 1 2 3))
))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(6));

	}

	{
		std::string program = R"((begin
(define f1 (lambda (x y z) (+ x (- y z))))
(apply f1 (list 1 3 2))
))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(2));

	}

	{
		std::string program = R"((begin
(define addtwo (lambda (x y) (+ x y)))
(apply addtwo (list 1 2 3))
))";
		INFO(program);
		Interpreter interp;
		std::istringstream iss(program);
		interp.parseStream(iss);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	
	}

}


TEST_CASE("Tests for map function", "[interpreter]") {

	//test that all semantic errors get thrown when needed
	{
		std::vector<std::string> programs = { "(map 2 1)",
			"(map 1)",
			"(map 1 (list 1 2))",
		"(map / (list (list 1 2) (list 1 2 3)))",
		"(map * (list (list 2 1) (list 3 4) (list 8 9)))" };

		for (auto s : programs) {
			INFO(s);
			Interpreter interp;
			std::istringstream iss(s);
			interp.parseStream(iss);
			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}

		std::string program = R"((begin
(define addtwo (lambda (x y) (+ x y)))
(map addtwo (list (list 1 2) (list 1 3)))
))";
		INFO(program);
		Interpreter interp;
		std::istringstream iss(program);
		interp.parseStream(iss);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}


	{
		std::string program = "(map - (list 1 2 3))";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(-1));
		expected.push_back(Expression(-2));
		expected.push_back(Expression(-3));
		REQUIRE(result == Expression(expected));

	}

	{
		std::string program = R"((begin
(define addtwo (lambda (x y) (+ x y)))
(define addtwofromlist (lambda (z) (addtwo (first z) (first (rest z)))))
(map addtwofromlist (list (list 1 2) (list 3 4)))
))";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(3));
		expected.push_back(Expression(7));
		REQUIRE(result == Expression(expected));

	}

	{
		std::string program = R"((begin
(define f (lambda (x) (list x (+ (* 2 x) 1))))
(map f (range -2 2 0.5))
))";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		std::vector<Expression> point1;
		std::vector<Expression> point2;
		std::vector<Expression> point3;
		std::vector<Expression> point4;
		std::vector<Expression> point5;
		std::vector<Expression> point6;
		std::vector<Expression> point7;
		std::vector<Expression> point8;
		std::vector<Expression> point9;
		point1.push_back(Expression(-2));
		point1.push_back(Expression(-3));
		point2.push_back(Expression(-1.5));
		point2.push_back(Expression(-2));
		point3.push_back(Expression(-1));
		point3.push_back(Expression(-1));
		point4.push_back(Expression(-.5));
		point4.push_back(Expression(0));
		point5.push_back(Expression(0));
		point5.push_back(Expression(1));
		point6.push_back(Expression(.5));
		point6.push_back(Expression(2));
		point7.push_back(Expression(1));
		point7.push_back(Expression(3));
		point8.push_back(Expression(1.5));
		point8.push_back(Expression(4));
		point9.push_back(Expression(2));
		point9.push_back(Expression(5));
		expected.push_back(point1);
		expected.push_back(point2);
		expected.push_back(point3);
		expected.push_back(point4);
		expected.push_back(point5);
		expected.push_back(point6);
		expected.push_back(point7);
		expected.push_back(point8);
		expected.push_back(point9);
		REQUIRE(result == Expression(expected));

	}

	{
		std::string program = "(map / (list 1 2 4))";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(1));
		expected.push_back(Expression(0.5));
		expected.push_back(Expression(0.25));
		REQUIRE(result == Expression(expected));

	}


}

TEST_CASE("Testing creation of strings", "[interpreter]") {

	{
		std::string program = "(define x \"foo\")";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(Atom("\"foo\"")));
	}

	{
		std::string program = "(list \"boo\" \"wow\" \"foo\")";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(Atom("\"boo\"")));
		expected.push_back(Expression(Atom("\"wow\"")));
		expected.push_back(Expression(Atom("\"foo\"")));
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(first (list \"boo\" \"wow\" \"foo\"))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(Atom("\"boo\"")));
	}

	{
		std::string program = "(rest (list \"boo\" \"wow\" \"foo\"))";
		INFO(program);
		Expression result = run(program);
		std::vector<Expression> expected;
		expected.push_back(Expression(Atom("\"wow\"")));
		expected.push_back(Expression(Atom("\"foo\"")));
		REQUIRE(result == Expression(expected));
	}

	
}

TEST_CASE("Testing set-property and get-property methods", "[interpreter]") {

	//test that all semantic errors get thrown when needed
	{
		std::vector<std::string> programs = { "(set-property 2 1)",
			"(set-property 1 2 3)",
			"(get-property 1 1)",
			"(get-property 1 2 3)"};

		for (auto s : programs) {
			INFO(s);
			Interpreter interp;
			std::istringstream iss(s);
			interp.parseStream(iss);
			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}

	{
	std::string program = R"((begin
(define a (+ 1 I))
(define b (set-property "note" "a complex number" a))
(get-property "note" b)
))";
	INFO(program);
	Expression result = run(program);
	REQUIRE(result == Expression(Atom("\"a complex number\"")));
	}

	


}

TEST_CASE("get back to 98 percent", "[interpreter]") {



	{
		Atom test;
		Expression exp(test);
		REQUIRE(exp.isHeadNone());
	}

	{
		Expression exp(1);
		REQUIRE(expString(exp) == "(1)");
	}

	{
		Expression exp;
		REQUIRE(expString(exp) == "NONE");
	}

	{
		Expression exp1(1);
		Expression exp2(2);
		std::vector<Expression> vec;
		vec.push_back(exp1);
		vec.push_back(exp2);
		Expression exp = Expression(vec);
		REQUIRE(expString(exp) == "((1) (2))");
	}



}

TEST_CASE("Testing discrete-plot", "[interpreter]") {

	{
		std::string program = R"(
    (discrete-plot (list (list -1 -1) (list 1 1)) 
    (list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label") ))
)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result.getTail().size() == 17);
	}

	{
		std::string program = R"(
    (discrete-plot (list (list -1 1) (list 1 3)) 
    (list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label") ))
)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result.getTail().size() == 16);
	}
}

TEST_CASE("Testing continuous-plot", "[interpreter]") {

	{
		std::string program = R"(
	(begin 
	(define f (lambda (x) (+ x 0)))
	(continuous-plot f (list -1 1) 
	(list (list "title" "A continuous linear function") 
	(list "abscissa-label" "x") 
	(list "ordinate-label" "y") )))
	)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result.getTail().size() == 63);
	}

	{
		std::string program = R"(
	(begin 
	(define f (lambda (x) (sin x)))
	(continuous-plot f (list (- pi) pi) 
	(list (list "title" "A continuous linear function") 
	(list "abscissa-label" "x") 
	(list "ordinate-label" "y") )))
	)";	INFO(program);
		Expression result = run(program);
		REQUIRE(result.getTail().size() == 75);
	}




}