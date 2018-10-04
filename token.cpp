#include "token.hpp"

// system includes
#include <cctype>
#include <iostream>

// define constants for special characters
const char OPENCHAR = '(';
const char CLOSECHAR = ')';
const char COMMENTCHAR = ';';
const char QUOTECHAR = '"';


Token::Token(TokenType t): m_type(t){}

Token::Token(const std::string & str){
	if (str[0] == '"') {
		m_type = QUOTE;
		value = str;
	}
	else {
		m_type = STRING;
		value = str;
	}
}

Token::TokenType Token::type() const{
  return m_type;
}

std::string Token::asString() const{
  switch(m_type){
  case OPEN:
    return "(";
  case CLOSE:
    return ")";
  case QUOTE:
	  return value;
  case STRING:
    return value;
  }
  return "";
}


// add token to sequence unless it is empty, clears token
void store_ifnot_empty(std::string & token, TokenSequenceType & seq){
  if(!token.empty()){
    seq.emplace_back(token);
    token.clear();
  }
}

TokenSequenceType tokenize(std::istream & seq){
  TokenSequenceType tokens;
  std::string token;
  bool quoteFlag = false;
  
  while(true){
    char c = seq.get();
    if(seq.eof()) break;
    
    if(c == COMMENTCHAR){
      // chomp until the end of the line
      while((!seq.eof()) && (c != '\n')){
		c = seq.get();
      }
      if(seq.eof()) break;
    }
    else if(c == OPENCHAR){
      store_ifnot_empty(token, tokens);
      tokens.push_back(Token::TokenType::OPEN);
    }
    else if(c == CLOSECHAR){
      store_ifnot_empty(token, tokens);
      tokens.push_back(Token::TokenType::CLOSE);
    }
	else if (c == QUOTECHAR) {
		if (quoteFlag) {
			token.push_back(c);
			store_ifnot_empty(token, tokens);
			quoteFlag = false;
			continue;
		}
		token.push_back(c);
		quoteFlag = true;
	}
    else if(isspace(c)){
		if (!quoteFlag) {
			store_ifnot_empty(token, tokens);
		}
		else {
			token.push_back(c);
		}
    }
    else{
      token.push_back(c);
    }
  }
  store_ifnot_empty(token, tokens);

  return tokens;
}
