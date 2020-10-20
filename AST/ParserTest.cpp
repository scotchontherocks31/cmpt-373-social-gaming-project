#include "Parser.h"
#include <nlohmann/json.hpp>
#include "ASTNode.h"


int main(){

    AST::DSLValue dsl{std::string{"Greeting Game"}};
    // create enviroment
    auto enviro = AST::Environment{nullptr};

    // insert DSL
    enviro.setBinding(std::string{"Game Name"}, dsl);

    // create communication obj
    AST::Communication comm{};

    // create interpreter with enviroment and communication
    AST::Interpreter interp = AST::Interpreter{std::move(enviro), comm};

    
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
    ast.getParent().    
    
    
    return 0;
}