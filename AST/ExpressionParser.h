#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>
enum class Type { // name it better
  OPENPAR,
  CLOSEPAR,
  ID,
  DOT,
  EQUALS,
  NOTEQUALS,
  GREATER,
  GREATEREQUALS,
  LESS,
  LESSEQUALS,
  NOT,
  COMMA,
};

static std::map<char, int> tokenTrigger =
    { // name it better... Change to vector
        {'(', 1}, {')', 1}, {'.', 1}, {'=', 2},
        {'!', 2}, {'<', 2}, {'>', 2}, {',', 1}};

static std::map<std::string, Type> stringToType = { // name it better
    {"(", Type::OPENPAR},
    {")", Type::CLOSEPAR},
    {".", Type::DOT},
    {"==", Type::EQUALS},
    {"!=", Type::NOTEQUALS},
    {">", Type::GREATER},
    {">=", Type::GREATEREQUALS},
    {"<", Type::LESS},
    {"<=", Type::LESSEQUALS},
    {"!", Type::NOT},
    {",", Type::COMMA}};

class TokenType {
public:
  TokenType(Type t, std::string s) : type{t}, value{s} {}
  std::pair<Type, std::string> getTypeValue() {
    return std::pair<Type, std::string>(type, value);
  }
  std::string getValue() { return value; }
  Type getType() { return type; }

private:
  Type type;
  std::string value;
};

enum class Terminal {
  COMMA,
  BIN,
  UN,
  OPENPAR,
  CLOSEPAR,
  ID,
  DOT,
  END,
};

static std::map<Type, Terminal> TypeToTerminal{
    // Anything that does not map is
    {Type::OPENPAR, Terminal::OPENPAR},   {Type::CLOSEPAR, Terminal::CLOSEPAR},
    {Type::DOT, Terminal::DOT},           {Type::EQUALS, Terminal::BIN},
    {Type::NOTEQUALS, Terminal::BIN},     {Type::GREATER, Terminal::BIN},
    {Type::GREATEREQUALS, Terminal::BIN}, {Type::LESS, Terminal::BIN},
    {Type::LESSEQUALS, Terminal::BIN},    {Type::NOT, Terminal::UN},
    {Type::COMMA, Terminal::COMMA},       {Type::ID, Terminal::ID}};

// The Main feature is to make sure i do not go out of bounds
struct Safeway {
  // I will use these Functions //
  Safeway(std::vector<TokenType> tokens) {
    size = tokens.size();
    currentIndex = 0;
    for(auto t : tokens){
      tokensQueue.push(t);
    }
    
    // Need a way for me to return Terminal::END at the very end
  }

  Terminal getTerminal() {
    if (currentIndex < size){
      return TypeToTerminal[tokensQueue.front().getType()];
    } else {
      return Terminal::END;
    }
    
  } // If it is out of bounds, return Terminal::END.
  TokenType front() { return tokensQueue.front(); }
  void next_token() { 
    tokensQueue.pop(); 
    currentIndex ++;
    }
  std::string getValue() { return tokensQueue.front().getValue(); }
  Type getType() { return tokensQueue.front().getType(); }
  // ----------------------------
private:
  int size ;
  int currentIndex;
  std::queue<TokenType> tokensQueue;
};

std::vector<TokenType> parseToType(std::string str);

#endif
