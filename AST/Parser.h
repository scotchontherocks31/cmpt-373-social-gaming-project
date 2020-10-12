#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "ASTNode.h"
#include "ASTVisitor.h"
#include <memory>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class JSON;

namespace AST {

class DomainSpecificParser {
    public:
        AST parse() {
            return parseHelper();
        }
        virtual ~DomainSpecificParser();
    private:
        virtual AST parseHelper() = 0;
};

class ASTParser {
    private:
        using DSP = DomainSpecificParser;
    public:
        ASTParser(std::unique_ptr<DSP>&& parser) : parser{std::move(parser)} {}
        AST parse() {
            return parser->parse();
        }
        void setParser(std::unique_ptr<DSP> &&parser) {
            parser.swap(this->parser);
        }
    private:
        std::unique_ptr<DSP> parser;
};

class JSONToASTParser : public DomainSpecificParser {
    public:
        JSONToASTParser(const json& json) : json{json} {}
    private:
        const json &json;
        // Implement these in a Top Down fashion
        virtual AST parseHelper() override;
        FormatNode parseFormatNode();
        GlobalMessage parseGlobalMessage();
};

}

#endif
