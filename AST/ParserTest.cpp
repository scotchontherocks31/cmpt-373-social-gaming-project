#include "Parser.h"
#include <nlohmann/json.hpp>
#include "ASTNode.h"


int main(){
    std::cout << "Starting test..." << std::endl;

    // create DSLValue
    AST::DSLValue dsl{std::string{"Greeting Game"}};
    // create enviroment
    AST::Environment enviro{nullptr};
    // insert DSL in environment
    enviro.setBinding(std::string{"Game Name"}, dsl);
    // create communication obj
    AST::Communication comm{};
    // create interpreter with enviroment and communication
    AST::Interpreter interp{std::move(enviro), comm};

    Json globalmessage= { 
        {"rule", "global-message"},
        {"value", "Great job!"}  
    };

    Json para = {
        {"rule", "parallelfor"},
        {"list", "players"},
        {"element", "player"},
        { "rules" , { globalmessage } }
    };

    Json rule = {
       { "rules" , { para, globalmessage} }
    };
    
    Json notGlobalmessage= {
        {"rule", "not-global-message"},
        {"value", "Not Great job!"}  
    };
    
    std::cout << "Got the JSON..." << std::endl;
    AST::JSONToASTParser JSONtoAST(rule);   //pass in JSON globalmessage
    AST::AST ast = JSONtoAST.parse();  //AST With GlobalMessage
    std::cout << "Got the JSON..." << std::endl;
    auto x = ast.accept(interp); 
    x.resume();
    
    return 0;
}