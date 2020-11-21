#include "ExpressionParser.h"

#include <algorithm>
#include <iostream>
#include <map>
//--------------------
Type getStringToType(std::string str) {
  if (stringToType.count(str) != 0) {
    return stringToType[str];
  } else {
    return Type::ID;
  }
}

int lookAhead(std::string str, int end) {
  if (str[end + 1] == '=') {
    end += 2;
  } else {
    end += 1;
  }
  return end;
}

TokenType toTokenType(std::string str, int begin, int end) {
  std::string input = str.substr(begin, (end - begin));
  Type type = getStringToType(input);
  return TokenType(type, input);
}

std::vector<TokenType> parseToType(std::string str) {
  std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
  str.erase(end_pos, str.end());

  std::vector<TokenType> tokens;

  int begin = 0;
  int end = 0;

  while (begin < str.length()) {

    if (tokenTrigger.count(str[end]) != 0) { // triggers on . , = , < , etc...
      if ((end - begin) == 0) { // if very next string is an opertor

        end = lookAhead(str, end);
        tokens.push_back(toTokenType(str, begin, end));
        begin = end;
      } else {
        tokens.push_back(toTokenType(str, begin, end));
        begin = end;

        end = lookAhead(str, end);

        tokens.push_back(toTokenType(str, begin, end));
        begin = end;
      }

    } else if (end >= str.length()) {
      tokens.push_back(toTokenType(str, begin, str.length()));
      break;
    } else {
      end++;
    }
  }
  return tokens;
}
