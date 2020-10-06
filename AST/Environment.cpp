#include "ASTVisitor.h"
#include "ASTNode.h"
#include <iostream>
#include <stdlib.h>

using namespace AST;
using namespace std;

//Car& Car::operator=(const Car &other) { }
Environment& Environment::operator=(Environment other)
{
    std::cout << "copy assignment of Enviroment\n";
    std::swap(bindings, other.bindings);
    std::swap(child), other.child);
    //std::swap(parent), other.parent);
    return *this;
}



void Environment::iam(){
    cout<<"i am enviroment\n";
}


void Environment::setBinding(ASTNode& node, DSLValue val){
    pair<ASTNode*, DSLValue> bindingPair (&node,val);
    bindings.insert (bindingPair); 

                         // copy insertion
    //bindings.insert (make_pair<ASTNode*, DSLValue>(node,val));
    //bindings[ASTNode&]= DSLValue;
    return;
}


