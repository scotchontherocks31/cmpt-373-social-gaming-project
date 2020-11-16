#include "ExpressionParser.h"

#include <algorithm>
#include <iostream>
#include <map>
//--------------------
Type getStringToType(std::string str) {
  // std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
  // str.erase(end_pos, str.end());
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
//--------------------------

std::vector<std::string> parseExpression(std::string str) {
  struct Visitor;
  using Expression = peg_parser::Interpreter<void, Visitor &>::Expression;

  struct Visitor {

    std::vector<std::string> orders;
    std::string result;

    std::string getValueString(Expression e) {
      // orders.push_back(e.string());
      return e.string();
      ;
    }
    void visitDot(Expression l, Expression r) {
      result = l.string() + "." + r.string();
      orders.push_back(result);
    }
    void visitComparison(Expression l, Expression r, std::string s) {
      orders.push_back(l.string());
      orders.push_back(s);
      orders.push_back(r.string());
    }
  };

  peg_parser::ParserGenerator<void, Visitor &> g;
  g.setSeparator(g["Whitespace"] << "[\t ]");
  g["Expression"] << "String";
  g["String"] << "Equals | Dots"; // 10

  g["Dots"] << "Dot | Atomic"; // 9

  g["Dot"] << "Dots '.' Atomic" >>
      [](auto e, auto &v) { v.visitDot(e[0], e[1]); }; // 8

  g["Equals"] << "String '==' Dots" >>
      [](auto e, auto &v) { v.visitComparison(e[0], e[1], "=="); }; // 7
  g["Greater"] << "String '>' Dots" >>
      [](auto e, auto &v) { v.visitComparison(e[0], e[1], ">"); }; // 7
  g["GreaterEquals"] << "String '>=' Dots" >>
      [](auto e, auto &v) { v.visitComparison(e[0], e[1], ">="); }; // 7
  g["Less"] << "String '<' Dots" >>
      [](auto e, auto &v) { v.visitComparison(e[0], e[1], "<"); }; // 7
  g["LessEquals"] << "String '<=' Dots" >>
      [](auto e, auto &v) { v.visitComparison(e[0], e[1], "<="); }; // 7

  g["Not"] << " '!' Equals"; // 0

  g["Brackets"] << "'(' String ')'";
  g["Name"] << "[a-zA-Z] [a-zA-Z0-9]*";
  g["Atomic"] << "Name | Brackets";
  // g["Function"] << " Name '(' Name ')' ";
  // g["And"]
  // g["or"]
  g.setStart(g["Expression"]);

  Visitor visitor;
  try {
    g.run(str, visitor);
    for (auto s : visitor.orders) {
      std::cout << s << std::endl;
    }
  } catch (peg_parser::SyntaxError &error) {
    // std::cout << "Syntax error while parsing " << error.syntax->rule->name <<
    // std::endl;
    std::cout << "Cannot be parsed further" << std::endl;
    std::vector<std::string> v = {str};
    return (v);
  }
  return visitor.orders;
}

/*

// call it inside the node
std::vector<std::string> expressionParser(std::string str){
    //in the expression node...
    //(winners.size.big.size == winners.size.small.fast) == true


    // winners.size.big.size
    // ==
    // winners.size.small.fast
    // ==
    // true

    // maybe make it recursive later
    std::vector<std::string> theList {str};
    std::vector<std::string> tempList;


    int prevSize = 1;
    do {
        for(int i = 0; i < theList.size(); i ++){
            tempList = parseExpression(theList[i]);
            theList.erase( theList.begin() + 1);
            theList.insert( theList.begin() + i , tempList.begin(),
tempList.end() ); // insert the returned parser from the string's original
position
        }
        prevSize = theList.size();
    } while (theList.size() != prevSize) ;


    return theList;
}

*/