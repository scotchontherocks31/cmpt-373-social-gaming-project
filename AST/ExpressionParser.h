#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H




#include <cmath>
#include <iostream>
#include <map>
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

static std::map<char, int> tokenTrigger = { // name it better... Change to vector
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

std::vector<TokenType> parseToType(std::string str);

struct RDP{ //Recursive Descent Parser
  
  enum class Terminal{
    COMMA,
    BIN,
    UN,
    OPENPAR,
    CLOSEPAR,
    ID,
    DOT,
  };

  std::map<Type, Terminal> TypeToTerminal{  //Anything that does not map is 
    {Type::OPENPAR , Terminal::OPENPAR},
    {Type::CLOSEPAR, Terminal::CLOSEPAR},
    {Type::DOT, Terminal::DOT},
    {Type::EQUALS, Terminal::BIN},
    {Type::NOTEQUALS, Terminal::BIN},
    {Type::GREATER, Terminal::BIN},
    {Type::GREATEREQUALS, Terminal::BIN},
    {Type::LESS, Terminal::BIN},
    {Type::LESSEQUALS, Terminal::BIN},
    {Type::NOT, Terminal::UN},
    {Type::COMMA, Terminal::COMMA},
    {Type::ID, Terminal::ID}
  };
  
  Terminal terminalType(TokenType t){
    if(TypeToTerminal.count(t.getType()) != 0){
      return TypeToTerminal[t.getType()];
    } 
  }

  
  std::vector<TokenType> tokens;
};


#endif
