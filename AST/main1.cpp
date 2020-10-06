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
    /*
    //ast accepts vistitor

    auto rootAST = AST{};

    auto interpVisitor = Interpreter{}

    rootAST.accept(interpVisitor)

    // -- INSIDE OF ASTNode accept function --
        void accept(ASTVisitor& visitor){
            auto children = this. getChildren();
            for child in children:
                visitor.visitEnter(child)
                visitor.visitLeave(child)



        }

    void accept(ASTVisitor& visitor);

    */

   // -- Enviroment

   auto enviro = Environment();
   enviro.iam();

   auto dsl = DSLValue{};
   
   auto globalOut = ASTGlobalOutNode("Welcome All");

   // creat dsl with "Welcome All"
   dsl.setString("Welcome ALL");


   // bind node and DSL in enviroment
   enviro.setBinding(globalOut,dsl);


   // put enviroment inside of visitor
   auto interp = Interpreter();

   interp.setEnvironment(enviro);
   //interp.environment = enviro;


   // use visitor to get
   



   







}