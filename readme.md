Introduction
-------------

Plotscript is a semester long software project for ECE 3574 written in C++. The starter code for the project implements the basic language interpreter for a language we will call Plot Script (plotscript) and uses a TUI to evaluate basic math functions such as addition, subtraction, and multiplication. Throughout the semester there were 5 milestones where I was asked to implement new functionality and expand on present functionality of the starter code.

Plot Script Overview
---------------------

Plot Script uses a prefix Lisp notation (also called [s-expressions](https://en.wikipedia.org/wiki/S-expression)). A plotscript program then is just one, possibly very complex, expression. For example the following program is roughly equivalent to the C++ one above.

```
(define x (/  (* (+ 1 2) 3) 4))
```

It computes the result of the numerical expression and assigns the symbol ``x`` to have that value in the _environment_. The environment is a mapping from known symbols to other expressions. When the program starts there is a default environment that gets updated as the program runs adding symbol-expression mappings.

Consider another example, a program that finds the max of two numbers:

```
(begin
 (define a 1)
 (define b pi)
 (if (< a b) b a)
 )
```
The outermost expression is a _special_ _form_ named ``begin``, that simply evaluates each argument in turn, itself evaluating to the last result. Its first argument is an expression that when evaluated adds a _symbol_ ``a`` to the _environment_ with a value of 1. The second argument of ``begin`` is another expression, this time a special form ``define``, that adds a symbol ``b`` to the environment with a value of the built-in symbol ``pi`` (that is, the symbol pi is in the default environment). The third argument is an expression that evaluates the expression ``(< a b)`` evaluating the expression ``b`` if the former evaluates to true, else evaluating the expression ``a``. In this case, since 1 < pi, the if expression evaluates to pi (3.14.159...). Notice white-space (e.g. tabs, spaces, and newlines) is unimportant in the syntax, but can be used to make the code more readable.

There are two equivalent views of the above syntax, as a list of lists or equivalently as a tree, called the _abstract_ _syntax_ _tree_ or AST.

<center>
![](ast.png)
</center>

When viewed as an AST,  the evaluation of the program (the outer-most expression), corresponds to a _post-order_ traversal of the tree, where the children are considered in order left-to-right. Each leaf expression evaluates to itself if it is a literal or, if a symbol, to the expression it maps to in the environment. Then the special-form or procedure is evaluated with its arguments. This continues in the post-order traversal until the root of the AST is evaluated, giving the final result of the program, in this case the expression consisting of a single atom, the numerical value of pi (the max of 1 and pi). If at any time during the traversal this process cannot be completed, the program is invalid and an error is emitted (this will be specified more concretely below). Such an invalid program might be syntactically correct, but not semantically correct. For example suppose the programmer forgot to define a value for ``a``, as in

```
(begin
 (define b pi)
 (if (< a b) b a)
 )
```
This is not a semantically valid program in our language and so interpreting it should produce an error.

The process of converting from the stream of text characters that constitute the candidate program to an AST is called _parsing_. This is typically broken up into two steps, tokenization and AST construction. The tokenization step converts the stream of characters into a list of tokens, where each token is a syntactically meaningful string. In our language this means splitting the stream into tokens consisting of ``(``, ``)``, or strings containing no white-space. For example the stream ``(+ a ( - 4))`` would become the list 

```
"(", "+", "a", "(", "-", "4", ")", ")"
```
This can be implemented as a finite state machine operating on the input stream to produce the token stream (see the Token module).

The process of AST construction then uses the token list to build the AST. Every time a ``(`` token is encountered a new node in the AST is created using the following token in the list. Its children are then constructed recursively in the same fashion. This is called a recursive descent parser since it builds the AST top-down in a recursive fashion. The provided parser is an iterative version of this algorithm (see the parse function).

Initial Plot Script Language Specification (starter code)
--------------------------------------------

Our initial plotscript language is relatively simple (you will be extending it during the course of the semester). It can be specified as follows.

An _Atom_ has a type and a value. The type may be one of None, Number, or Symbol. The type ``None`` indicates the expression has no value. The possible values of a Number are any IEEE double floating point value, strictly parsed with no trailing characters. The possible values of a Symbol is any string, not containing white-space, not possible to parse as a Number, not beginning with a numerical digit, and not one of the _special_ _forms_ defined below.

Examples of Numbers are: ``1``, ``6.02``, ``-12``, ``1e-4``

Examples of Symbols are: ``a``, ``length``, ``start``

An _Expression_ is an Atom or a special form, followed by a (possibly empty) list of Expressions surrounded by parenthesis and separated by spaces. When an expression consists only of an atom the parenthesis may be omitted.

* ``<atom>``
* ``(<atom>)``
* ``(<atom> <expression> <expression> ...)``

There are two special-form expressions that begin with ``define``, and ``begin``. All other expressions are of the form ``(<symbol> <expression> <expression> ...)`` where the symbol names a _procedure_. Procedures take the one or more arguments and return an expression according to their name. For example the expression ``(+ a b c)`` where ``a``, ``b``, and ``c`` are atoms representing numbers or expressions evaluating to such an atom and the result is an expression consisting of a single number atom. This expression is _m-ary_ meaning it can take m arguments. Some expressions are binary, meaning they take only two arguments, i.e. ``(- a b)`` subtracts b from a. Other are unary, e.g. ``(- 1)``. Thus all procedures have an _arity_ of 1,2,...m.

The _Environment_ is a mapping from symbols to either an Expression or a built-in Procedure. The process of _evaluating_ an Expression may modify the Environment (a side-effect) and results in an expression, which consists of a single Atom.

Our language has the following special-forms:

* ``(define <symbol> <expression>)`` adds a mapping from the symbol to the result of the expression in the environment. It is an error to redefine a symbol. This evaluates to the expression the symbol is defined as (maps to in the environment).
* ``(begin <expression> <expression> ...)`` evaluates each expression in order, evaluating to the last.

Our language has the following built-in procedures:

* ``+``, m-ary expression of Numbers, returns the sum of the arguments
* ``-``, unary expression of Numbers, returns the negative of the argument
* ``-``, binary expression of Numbers, return the first argument minus the second
* ``*``, m-ary expression of Number arguments, returns the product of the arguments
* ``/``, binary expression of Numbers, return the first argument divided by the second

It is an error to evaluate a procedure with an incorrect arity or incorrect argument type.

Our language has the following built-in symbol:

* ``pi``, a Number, evaluates to the numerical value of pi, given by atan2(0, -1)

Our language also supports comments using the traditional lisp notation. Any content after and including the character ``;`` up to a newline is considered a comment and ignored by the parser (actually the tokenizer).

See the directory ``tests`` in the repository an example plotscript program demonstrating the above syntax.

Milestones
--------

Miletone 0: In this milestone I added new built in math functions for the plotscript program including Euler's number, square root, exponents, natural log, sin, cos, and tan. I also added functionality for complex numbers in the new and existing functions.

Miletone 1: The first step in this milestone was to implement list functioanlity in the program. The existing code had to be manipulated to evaluate new list functions and existing functions with lists as inputs. New built in functions were added to both apply built in functions onto a list and use a list as an input to built in functions. After completing this, I edited the program to allow for user-defined procedures called lambda functions. This allows the user to define their own functions and then evaluate them in the TUI. This required substantial reconstruction of the environment and expression modules.

Miletone 2: This milestone began the process of adding graphing primitives that would then become useful upon building the GUI. I first added a map to each expression where a property value could be set. Then I created a start up file with pre-defined functions called make-text, make-line, and make-point with the necessary default properties that they would have upon starting the program.

Miletone 3: This milestone implemented the GUI for the program. The GUI has an input widget that inherits from QT's QPlainTextEdit module to create a textbox for inputting a plotscript expression. It also has an output widget that uses composition to create a graphics scene that can display text and shapes. The GUI can be used to evaluate regular plotscript expressions or create graphs from lambda functions.

Miletone 4: This milestone began by creating a thread safe queue that would take in plotscript inputs and evaluate them one at a time. This was implemented in both the TUI and GUI to create separate kernels for each. In the TUI, the commands %start, %stop, and %reset were added to start, stop, and reset the kernel. This functionality was added to the top of the GUI using push buttons.

*Note unit tests were written during each module to keep up to date confimation of correct functionality

Modules
--------

The C++ code implementing the plotscript interpreter is divided into the following modules, consisting of a header and implementation pair (.hpp and .cpp). See the associated linked pages for details.

From the starter code:

* Atom Module (``atom.hpp``, ``atom.cpp``): This module defines the variant type used to hold Atoms.
* Expression Module (``expression.hpp``, ``expression.cpp``): This module defines a class named ``Expression``, forming a node in the AST.
* Tokenize Module (``token.hpp``, ``token.cpp``): This module defines the C++ types and code for lexing (tokenizing).
* Parsing Module (``parse.hpp``, ``parse.cpp``): This defines the parse function.
* Environment Module (``environment.hpp``, ``environment.cpp``): This module defines the C++ types and code that implements the plotscript environment mapping.
* Interpreter Module (``interpreter.hpp``, ``interpreter.cpp``):  This module implements a class named "Interpreter`` for parsing and evaluation of the AST representation of the expression.
	
Added throughout the milestones:

* Input Widget Module (``input_widget.hpp``, ``input_widget.cpp``): This module uses the QT framework to create a textbox where the user can input a plotscript expression.
* Output Widget Module (``output_widget.hpp``, ``output_Widget.cpp``): This module uses the QT framework to create a graphics scene that can display text for the result of an expression or error, or graphs for the added graphing functions.
* Notebook App Module (``notebook_app.hpp``, ``notebook_app.cpp``): This module uses the input widget and output widget, plus 3 buttons for kernel activity to create the GUI for the program.
* Thread Safe Queue Module (``ThreadSafeQueue.cpp``): This module defines a thread safe queue class to allow for concurrency in the program using threads.
	
