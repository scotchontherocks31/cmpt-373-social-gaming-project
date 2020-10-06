

#include "ASTVisitor.h"
#include <iostream>
#include <stdlib.h>

using namespace AST;
using namespace std;


void DSLValue::iamDSL(){
    cout<<"I am dsl\n";
}

void DSLValue::setString(string dslStr){
    dslString = dslStr;
}