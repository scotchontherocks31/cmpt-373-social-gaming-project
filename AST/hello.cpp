/**
 *  This example demonstrate how we can use peg_parser::parser to define a
 * command-line calculator and use a visitor pattern to evaluate the result.
 */

#include <peg_parser/generator.h>

#include <cmath>
#include <iostream>
#include <unordered_map>
#include <vector>

int main() {
  
  using namespace std;

  struct Visitor;

  using Expression = peg_parser::Interpreter<void, Visitor &>::Expression;

  struct Visitor {
    std::vector<std::string> orders;
    std::string result;

    std::string getValueString(Expression e) {
      orders.push_back(e.string());
      return e.string();;
    }

    void visitDot(Expression l, Expression r) { result = getValueString(l) +  getValueString(r); }
    void visitEquals(Expression l, Expression r) { result = getValueString(l) +  getValueString(r); }
    
  };

  peg_parser::ParserGenerator<void, Visitor &> calculator;
  
  auto &g = calculator;
  g.setSeparator(g["Whitespace"] << "[\t ]");
  g["Expression"] << "String";
  g["String"] << "Equals | Dots";  //sum

  g["Dots"] << "Dot | Atomic";  //product

  g["Dot"] << "Dots '.' Atomic" >> [](auto e, auto &v) { v.visitDot(e[0], e[1]); }; //multiply

  g["Equals"] << "String '==' Dots" >> [](auto e, auto &v) { v.visitEquals(e[0], e[1]); };  //subtract
  
  g["Brackets"] << "'(' String ')'";
  g["Name"] << "[a-zA-Z] [a-zA-Z0-9]*";
  g["Atomic"] << "Name | Brackets";
  g.setStart(g["Expression"]);

  cout << "Enter an expression to be evaluated.\n";

  
    //string str = "5 + 3 - 2 * 2 / 2";
    string str = "winners.size.big.size == winners.size.small.fast";
    try {



      Visitor visitor;
      calculator.run(str, visitor);
      //cout << str << " = " << visitor.result << endl;
      for(auto s: visitor.orders){
        cout << s << endl;
      }




    } catch (peg_parser::SyntaxError &error) {
      auto syntax = error.syntax;
      cout << "  ";
      cout << string(syntax->begin, ' ');
      cout << string(syntax->length(), '~');
      cout << "^\n";
      cout << "  "
           << "Syntax error while parsing " << syntax->rule->name << endl;
    }
  

  return 0;
}
