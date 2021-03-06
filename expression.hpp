/*! \file expression.hpp
Defines the Expression type and assiciated functions.
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>
#include <map>
#include <string>

#include "token.hpp"
#include "atom.hpp"

// forward declare Environment
class Environment;

/*! \class Expression
\brief An expression is a tree of Atoms.

An expression is an atom called the head followed by a (possibly empty) 
list of expressions called the tail.
 */
class Expression {
public:

  typedef std::vector<Expression>::const_iterator ConstIteratorType;

  /// Default construct and Expression, whose type in NoneType
  Expression();

  ///Constructor for creating list type
  Expression(const std::vector<Expression> & args);

  ///Constructor for creating lambda function from two Expressions, the arguments and the function
  Expression(const Expression & tail0, const Expression & tail1);

  /*! Construct an Expression with given Atom as head an empty tail
    \param atom the atom to make the head
  */
  Expression(const Atom & a);

  /// deep-copy construct an expression (recursive)
  Expression(const Expression & a);

  /// deep-copy assign an expression  (recursive)
  Expression & operator=(const Expression & a);

  /// return a reference to the head Atom
  Atom & head();

  /// return a const-reference to the head Atom
  const Atom & head() const;

  /// append Atom to tail of the expression
  void append(const Atom & a);

  /// append Expression to tail of the expression
  void append(const Expression & e);

  /// return a pointer to the last expression in the tail, or nullptr
  Expression * tail();

  /// return a const-iterator to the beginning of tail
  ConstIteratorType tailConstBegin() const noexcept;

  /// return a const-iterator to the tail end
  ConstIteratorType tailConstEnd() const noexcept;

  /// convienience member to determine if head atom is a number
  bool isHeadNumber() const noexcept;

  /// convienience member to determine if head atom is a symbol
  bool isHeadSymbol() const noexcept;

  /// convienience member to determine if head atom is a complex
  bool isHeadComplex() const noexcept;

  /// convienience member to determine if head atom is a string
  bool isHeadString() const noexcept;

  /// convienience member to determine if head atom is a none kind
  bool isHeadNone() const noexcept;

  /// convienience member to determine if head atom is a none kind
  bool isHeadLambda() const noexcept;

  /// convienience member to determine if head atom is a none kind
  bool isHeadList() const noexcept;

  /// Evaluate expression using a post-order traversal (recursive)
  Expression eval(Environment & env);

  /// equality comparison for two expressions (recursive)
  bool operator==(const Expression & exp) const noexcept;

  std::vector<Expression> getTail() const noexcept;

  size_t tailLength() const noexcept;

  Expression getProperty(std::string key);

  void setProperty(std::string key, Expression val);

  void setPropertyList(std::map<std::string,Expression> map);

  std::map<std::string, Expression> getPropertyList();
  
private:

  // the head of the expression
  Atom m_head;

  // the tail list is expressed as a vector for access efficiency
  // and cache coherence, at the cost of wasted memory.
  std::vector<Expression> m_tail;

  std::map<std::string, Expression> property_list;

  // convenience typedef
  typedef std::vector<Expression>::iterator IteratorType;
  
  // internal helper methods
  Expression handle_lookup(const Atom & head, const Environment & env);
  Expression handle_define(Environment & env);
  Expression handle_begin(Environment & env);
  Expression handle_lambda();
};

/// Render expression to output stream
std::ostream & operator<<(std::ostream & out, const Expression & exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression & left, const Expression & right) noexcept;

bool operator<(const Expression & left, const Expression & right) noexcept;

std::string expString(Expression& exp);
  
#endif
