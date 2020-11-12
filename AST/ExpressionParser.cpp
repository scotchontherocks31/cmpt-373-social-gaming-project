#include "ExpressionParser.h"

std::vector<std::string> parseExpression(std::string str){
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

    peg_parser::ParserGenerator<void, Visitor &> g;
    g.setSeparator(g["Whitespace"] << "[\t ]");
    g["Expression"] << "String";
    g["String"] << "Equals | Dots";  //10

    g["Dots"] << "Dot | Atomic";  //9

    g["Dot"] << "Dots '.' Atomic" >> [](auto e, auto &v) { v.visitDot(e[0], e[1]); }; //8

    g["Equals"] << "String '==' Dots" >> [](auto e, auto &v) { v.visitEquals(e[0], e[1]); };  //7
    
    g["Brackets"] << "'(' String ')'";
    g["Name"] << "[a-zA-Z] [a-zA-Z0-9]*";
    g["Atomic"] << "Name | Brackets";
    g.setStart(g["Expression"]);

    Visitor visitor;
    try {
        g.run(str, visitor);
        for(auto s: visitor.orders){
            std::cout << s << std::endl;
        }
    } catch (peg_parser::SyntaxError &error) {
      std::cout << "Syntax error while parsing " << error.syntax->rule->name << std::endl;
    }
    return visitor.orders;
}
