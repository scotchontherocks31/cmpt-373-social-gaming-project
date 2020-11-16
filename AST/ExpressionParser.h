#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <peg_parser/generator.h>

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

static std::map<char, int> tokenTrigger = { // name it better
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

enum class OperationType {
  DOT,
  EQUALS,
  GREATER,
  GREATEREQUALS,
  LESS,
  LESSEQUALS,
  NOT,
  NOTHING,
};

static std::map<std::string, OperationType> stringToOperator = {
    {".", OperationType::DOT},     {"==", OperationType::EQUALS},
    {">", OperationType::GREATER}, {">=", OperationType::GREATEREQUALS},
    {"<", OperationType::LESS},    {"<=", OperationType::LESSEQUALS},
    {"!", OperationType::NOT}};

std::vector<std::string> parseExpression(std::string str);
std::vector<std::string> expressionParser(std::string str);
std::vector<TokenType> parseToType(std::string str);
#endif