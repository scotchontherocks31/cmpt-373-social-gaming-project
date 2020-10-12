#include "Parser.h"

namespace AST{

    //I'm assuming this to setup initial AST 
    //with a single root node inside of it
    AST JSONToASTParser::parseHelper(){
        
        if(this->json["rule"].contains("global-message")){
            GlobalMessage node = parseGlobalMessage();
            std::unique_ptr<GlobalMessage> glbl_prt(&node);
            AST ast(std::move(glbl_prt));
        }

        //else, funnel in a format root node
        FormatNode root(this->json["rule"]);
        std::unique_ptr<FormatNode> root_ptr(&root);
        AST ast(std::move(root_ptr));
    }

    FormatNode JSONToASTParser::parseFormatNode(){
        //assuming json obj. has a "rules" string
        FormatNode format(this->json["rule"]);
        return format;
    }

    GlobalMessage JSONToASTParser::parseGlobalMessage(){

        FormatNode format(this->json["rule"]);
        std::unique_ptr<FormatNode> format_ptr(&format);

        GlobalMessage result(std::move(format_ptr));

        return result;
    }
}