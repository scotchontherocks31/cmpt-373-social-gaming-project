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
   
  
    // create dsl, any variable, ex. Name of game
    DSLValue dsl{std::string{"Greeting Game"}};
    
    // create enviroment
    auto enviro = Environment{nullptr};

    //insert DSL
    enviro.setBinding(std::string{"Game Name"},dsl );

    // create communication obj
    Communication comm{};


    // create interpreter with enviroment and communication
    Interpreter interp = Interpreter{std::move(enviro),comm};
    // call visitEnter

    // create temp global message node
   
   GlobalMessage mess = GlobalMessage{std::make_unique<FormatNode>(std::string{"Welcome All "})};
   
   
   mess.accept(interp);
   

    
}



   







