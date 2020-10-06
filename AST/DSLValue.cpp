

#include "ASTVisitor.h"
#include <iostream>
#include <string>
using namespace AST;
using namespace std;


void DSLValue::iamDSL(){
    cout<<"I am dsl\n";
}

std::string DSLValue::getString(){
    return dslString;
}

void DSLValue::setString(string dslStr){
    dslString = dslStr;
}