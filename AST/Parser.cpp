#include "Parser.h"
#include <assert.h>

namespace AST{

    AST JSONToASTParser::parseHelper(){
        /*Previous test case codes
        assert(json["rules"]); // "rules" is implemented later when "foreach" , "parallefor" , etc is made
        auto &&globalMessage = parseGlobalMessage(json["rule"]);
        return AST{std::move(globalMessage)}; 

        std::unique_ptr<FormatNode> format = std::make_unique<FormatNode>(json["value"]);
        std::unique_ptr<GlobalMessage> globalMessage = std::make_unique<GlobalMessage>(std::move(format));
        return AST{std::move(globalMessage)};
        */
        std::cout<< "parsehelper..." << std::endl;
        return AST{std::move(parseRules(this->json["rules"]))};
    }

    std::unique_ptr<Rules> JSONToASTParser::parseRules(const Json& json){
        
        auto rulePtr = std::make_unique<Rules>();
        for(const auto& rule : json){
            std::cout << rule.dump(4) << std::endl;
            rulePtr->appendChild(parseRule(rule));
        }
        std::cout << "You did it! END loop" << std::endl;
        return rulePtr;
    }

    std::unique_ptr<ASTNode> JSONToASTParser::parseRule(const Json& json){
        std::cout<< "parserule..." << std::endl;
        //assert(json["rule"]);
        if(json["rule"] == "global-message"){
             std::cout<< "global-message..." << std::endl;
            return parseGlobalMessage(json);
        }
        else if(json["rule"] == "parallelfor"){
             std::cout<< "parallelfor..." << std::endl;
            return parseParallelFor(json);
        }
        std::cout <<"Reached nothing" << std::endl;
    }

    std::unique_ptr<FormatNode> JSONToASTParser::parseFormatNode(const Json& json){
        std::cout<< "parseformat..." << std::endl;
        //assert(json["value"]);
        return std::make_unique<FormatNode>(json["value"]);
    }

    std::unique_ptr<GlobalMessage> JSONToASTParser::parseGlobalMessage(const Json& json){
        std::cout<< "parseglobal..." << std::endl;
        auto &&format = parseFormatNode(json);
        return std::make_unique<GlobalMessage>(std::move(format));
    }

    std::unique_ptr<ParallelFor> JSONToASTParser::parseParallelFor(const Json& json){
        std::cout<< "parseparallel..." << std::endl;
        auto &&var = parseVariable(json);
        auto &&varDec = parseVarDeclaration(json);
        auto &&rules = parseRules(json["rules"]);

        return std::make_unique<ParallelFor>(std::move(var), std::move(varDec), std::move(rules));
    }

    std::unique_ptr<VarDeclaration> JSONToASTParser::parseVarDeclaration(const Json& json){
        std::cout<< "parsedecvar..." << std::endl;
        //assert(json["element"]);
        return std::make_unique<VarDeclaration>(json["element"]);
    }
   
    std::unique_ptr<Variable> JSONToASTParser::parseVariable(const Json& json){
         std::cout<< "parsevar..." << std::endl;
        //assert(json["list"]);
        return std::make_unique<Variable>(json["list"]);
    }
}