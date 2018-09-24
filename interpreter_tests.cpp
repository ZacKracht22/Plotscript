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
  
  std::vector<std::string> programs = {"(@ none)", // no such procedure
				       "(- 1 1 2)", // too many arguments
				       "(define begin 1)", // redefine special form
				       "(define pi 3.14)", // redefine builtin symbol
					   "(define I (sqrt (- 1)))", // redefine builtin symbol
					   "(+ 1 2 3 @)", //can't add a symbol
					   "(* 1 2 3 @)", //can't multiply a symbol
					   "(- 1 2 3)", //can't subtract more than 2 numbers
					   "(- @)", //can't negate a symbol
					   "(- 1 @)", //can't subtract a symbol
					   "(/ 1 2 3 4)", //can't divide more than 2 numbers
					   "(/ 1 !)", //can't divide a symbol
					   "(sqrt 2 3)", //can't call sqrt with more than 1 arg
					   "(pow 1 2 3)", //can't call pow with more than 2 args
					   "(ln (- 5))", //can't call ln with a negative number
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
					    "(conj I I I)" }; //cant call conj with multiple arguments
    for(auto s : programs){
      Interpreter interp;

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

	//test that the length procedure works for lists of numbers
	{
		std::string program = "(length (list))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression());
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