#include "ASTVisitor.h"
#include <iostream>
using namespace std;
using namespace AST;



/*

void Interpreter::setEnvironment(Environment& enviro){
    //error?
    environment = enviro;
}
*/

Interpreter::Interpreter(Environment &enviro) : environment(enviro) {};


void Interpreter::visitEnter(ASTGlobalOutNode& node) {

    // get value from enviroment
    DSLValue& val = environment.getValue(node);

    std::cout<<val.getString()<<endl;



    //send message
}


/*
void Interpreter::visitLeave(ASTGlobalOutNode&) {

    
};
*/