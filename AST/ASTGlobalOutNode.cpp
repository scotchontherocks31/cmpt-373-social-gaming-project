#include "ASTNode.h"
#include "ASTGlobalOutNode.h"
#include <iostream>
#include <stdlib.h>


using namespace std;


ASTGlobalOutNode::ASTGlobalOutNode(const std::string& mess){
    cout <<"sending message\n"<<mess<<endl;
}


