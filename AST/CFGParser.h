#ifndef CFG_PARSER_H
#define CFG_PARSER_H

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

static std::map<Type, std::string> typeToString = { // name it better
    {Type::OPENPAR, "("},
    {Type::CLOSEPAR, ")"},
    {Type::DOT, "."},
    {Type::EQUALS, "=="},
    {Type::NOTEQUALS, "!="},
    {Type::GREATER, ">"},
    {Type::GREATEREQUALS, ">="},
    {Type::LESS, "<"},
    {Type::LESSEQUALS, "<="},
    {Type::NOT, "!"},
    {Type::COMMA, ","}};

class CFGExpression {
public:
  CFGExpression(Type t, std::string s) : type{t}, value{s} {}
  std::pair<Type, std::string> getTypeValue() {
    return std::pair<Type, std::string>(type, value);
  }
  std::string getValue() { return value; }
  Type getType() { return type; }

private:
  Type type;
  std::string value;
};

std::vector<CFGExpression> parseToCFGExpression(std::string str);
#endif
