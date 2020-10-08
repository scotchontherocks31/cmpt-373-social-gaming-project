#include <iostream>
#include <stdlib.h>
#include <string>
#include "ASTVisitor.h"
#include "ASTGlobalOutNode.h"
#include "ASTNode.h"


//using namespace std;
using namespace AST;

int main()
{
    // run with: g++ -std=c++20 -o main1 main1.cpp Environment.cpp DSLValue.cpp ASTGlobalOutNode.cpp Interpreter.cpp

    std::cout << "Visitor simulator\n";
   
  

   
    //DSLValue a = List{"me", List{1, 2, 3}, 7.1, 1, Map{{"hello", 2}}};
    //a = Map{{"hello", 2}, {"nomore", 3}};
    /*
    DSLValue a{std::string{"bla"}}, b{2};
    a = 2;
    a = "123";
    a = std::string("blabli");
    a = std::vector<DSLValue>{std::string("bla"), 2.3, 2};
    List p = a.get<List>();
    DSLValue c{a};
    std::string &f = c.get<std::string>();
    */
    // create dsl
    DSLValue dsl{std::string{"Welcome All"}};
    


    // create enviroment
    //Environment enviro{NULL};
    auto enviro = std::make_unique<Environment>(nullptr);
    //std::unique_ptr<Environment> enviro{};

    //insert DSL
    //enviro->setBinding(std::string{"welcome"},dsl );

    // create communication obj
    Communication comm{};


    // create interpreter with enviroment and communication
    Interpreter interp = Interpreter{std::make_unique<Environment>(nullptr),comm};
    // call visitEnter

        // use enviroment to get DSL value
        // use communication to send dsl value






    
}



   







