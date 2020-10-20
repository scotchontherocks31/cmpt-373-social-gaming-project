#include "Parser.h"
#include <assert.h>

namespace AST{

    AST JSONToASTParser::parseHelper(){
        //assert(json["rules"]); "rules" is implemented later probably when "foreach" or "parallefor" is made
        //auto &&globalMessage = parseGlobalMessage(json["rule"]);
        //return AST{std::move(globalMessage)}; 
    
        std::unique_ptr<FormatNode> format = std::make_unique<FormatNode>(json["value"]);
        std::unique_ptr<GlobalMessage> globalMessage = std::make_unique<GlobalMessage>(std::move(format));
        return AST(std::move(globalMessage));
        
    }

    /*
    std::unique_ptr<FormatNode> JSONToASTParser::parseFormatNode(const Json& json){
        assert(json["value"]);
        return std::make_unique<FormatNode>(this->json["value"]);
    }

    std::unique_ptr<GlobalMessage> JSONToASTParser::parseGlobalMessage(const Json& json){
        assert(json["rule"]);                         
        assert(json["rule"].contains("global-message"));

        auto &&format = parseFormatNode(json);
        return std::make_unique<GlobalMessage>(std::move(format));
    }
    */
}