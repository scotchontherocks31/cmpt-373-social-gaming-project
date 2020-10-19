#include "Parser.h"
#include <nlohmann/json.hpp>
#include "ASTNode.h"
using Json = nlohmann::json;

int main(){
    Json globalmessage= { 
        {"rule", "global-message"},
        {"value", "Great job!"}  
    };

    Json notGlobalmessage= {
        {"rule", "not-global-message"},
        {"value", "Not Great job!"}  
    };

    AST::JSONToASTParser JSONtoAST(globalmessage);   //pass in JSON globalmessage
    AST::AST ast = JSONtoAST.parse();      
    
    std::cout << ast.getParent().getChildrenCount();
    return 0;
}