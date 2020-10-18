#include "Parser.h"
#include <assert.h>

namespace AST{

    AST JSONToASTParser::parseHelper(){
        assert(json["rules"]);
        auto &&globalMessage = parseGlobalMessage(json["rules"]);
        return AST{std::move(globalMessage)};   
    }

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
}