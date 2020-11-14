#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <peg_parser/generator.h>

#include <cmath>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

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

#endif