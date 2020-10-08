#include <iostream>
#include <stdlib.h>
#include <string>
#include "ASTVisitor.h"
#include "ASTGlobalOutNode.h"
#include "ASTNode.h"


using namespace std;
using namespace AST;

int main()
{
    // run with: g++ -o main1 main1.cpp Environment.cpp DSLValue.cpp ASTGlobalOutNode.cpp Interpreter.cpp

    cout << "Visitor simulator\n";
   
   // -- Enviroment
    /*
   auto enviro = Environment();
   enviro.iam();

   auto dsl = DSLValue{};
   
   auto globalOut = ASTGlobalOutNode("Welcome All");

   // creat dsl with "Welcome All"
   dsl.setString("Welcome ALL");


   // bind node and DSL in enviroment
   enviro.setBinding(globalOut,dsl);


    ASTVisitor* vist = new Interpreter{enviro}; 
 

   // put enviroment inside of visitor
   //ASTVisitor* interp = &Interpreter(enviro);


    vist->visitEnter(globalOut);

    */

   







}