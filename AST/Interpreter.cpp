#include "ASTVisitor.h"

using namespace AST;





void Interpreter::setEnvironment(Environment& enviro){
    //error?
    this->environment = enviro;
}

void Interpreter::visitLeave(ASTGlobalOutNode&) {

    
};


void Interpreter::visitEnter(ASTGlobalOutNode&) {

}

