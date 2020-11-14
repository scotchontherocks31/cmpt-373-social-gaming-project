#include "ExpressionParser.h"

#include <map>

std::vector<std::string> parseExpression(std::string str){
    struct Visitor;
    using Expression = peg_parser::Interpreter<void, Visitor &>::Expression;

    struct Visitor {
       
        std::vector<std::string> orders;
        std::string result;

        std::string getValueString(Expression e) {
        //orders.push_back(e.string());
        return e.string();;
        }
        void visitDot(Expression l, Expression r) { 
            result = l.string() +  "." + r.string(); 
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
    g["String"] << "Equals | Dots";  //10

    g["Dots"] << "Dot | Atomic";  //9

    g["Dot"] << "Dots '.' Atomic" >> [](auto e, auto &v) { v.visitDot(e[0], e[1]); }; //8

    g["Equals"] << "String '==' Dots" >> [](auto e, auto &v) { v.visitComparison(e[0], e[1], "==" ); };         //7
    g["Greater"] << "String '>' Dots" >> [](auto e, auto &v) { v.visitComparison(e[0], e[1], ">" ); };         //7
    g["GreaterEquals"] << "String '>=' Dots" >> [](auto e, auto &v) { v.visitComparison(e[0], e[1], ">="); };  //7
    g["Less"] << "String '<' Dots" >> [](auto e, auto &v) { v.visitComparison(e[0], e[1], "<"); };            //7
    g["LessEquals"] << "String '<=' Dots" >> [](auto e, auto &v) { v.visitComparison(e[0], e[1], "<="); };     //7

    g["Not"] << " '!' Equals";  // 0
    
    g["Brackets"] << "'(' String ')'";
    g["Name"] << "[a-zA-Z] [a-zA-Z0-9]*";
    g["Atomic"] << "Name | Brackets";
    //g["Function"] << " Name '(' Name ')' ";
    g.setStart(g["Expression"]);

    Visitor visitor;
    try {
        g.run(str, visitor);
        for(auto s: visitor.orders){
            std::cout << s << std::endl;
        }
    } catch (peg_parser::SyntaxError &error) {
      //std::cout << "Syntax error while parsing " << error.syntax->rule->name << std::endl;
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
            theList.insert( theList.begin() + i , tempList.begin(), tempList.end() ); // insert the returned parser from the string's original position
        }
        prevSize = theList.size();
    } while (theList.size() != prevSize) ;

    
    return theList;
}

*/