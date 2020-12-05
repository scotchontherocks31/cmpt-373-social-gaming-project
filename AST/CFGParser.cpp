#include "CFGParser.h"
#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <regex>
//--------regex------------

std::regex ID{"^[A-Za-z0-9]+"};

std::regex DOT{"^[.]"}; // These 3 MUST be in Square Brackets, beccause they
                        // have a special case with round bracket
std::regex OPENPAR{"^[(]"};
std::regex CLOSEPAR{"^[)]"};

std::regex EQUALS{"^(==)"};
std::regex NOTEQUALS{"^(!=)"};
std::regex NOT{"^(!(?!=))"};
std::regex GREATER{"^(>(?!=))"};
std::regex GREATEREQUALS{"^(>=)"};
std::regex LESS{"^(<(?!=))"};
std::regex LESSEQUALS{"^(<=)"};
std::regex COMMA{"^(,)"};

//--------------------------

class myexception : public std::exception {
  virtual const char *what() const throw() { return "Expression is invalid"; }
};

std::vector<std::regex> expressionList = {
    ID,      DOT,           OPENPAR, CLOSEPAR,   EQUALS, NOTEQUALS,
    GREATER, GREATEREQUALS, LESS,    LESSEQUALS, NOT,    COMMA};

Type getStringToType(std::string str) {
  if (stringToType.count(str) != 0) {
    return stringToType[str];
  } else {
    return Type::ID;
  }
}

std::optional<std::string> getFirstMatch(std::string str) {
  std::smatch match;

  for (auto &expr : expressionList) {
    if (std::regex_search(str, match, expr)) {
      return match.str(0);
    }
  }
  return std::nullopt;
}

std::vector<CFGExpression> parseToCFGExpression(std::string str) {
  std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
  str.erase(end_pos, str.end());
  std::vector<CFGExpression> tokens;

  while (!str.empty()) {
    try {
      std::string match = getFirstMatch(str).value();
      CFGExpression t(getStringToType(match), match);
      tokens.push_back(t);
      str = str.substr(match.length());

    } catch (const std::bad_optional_access &e) { // nullopt
      throw myexception();
    }
  }

  return tokens;
}
