#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "ASTNode.h"
#include "ASTVisitor.h"
#include <memory>
#include "json.hpp"
using Json = nlohmann::json;

namespace AST {

class DomainSpecificParser {
    public:
        AST parse() {
            return parseHelper();
        }
        virtual ~DomainSpecificParser() {}
    private:
        virtual AST parseHelper() = 0;
};

class ASTParser {
private:
  using DSP = DomainSpecificParser;

public:
  ASTParser(std::unique_ptr<DSP> &&parser) : parser{std::move(parser)} {}
  AST parse() { return parser->parse(); }
  void setParser(std::unique_ptr<DSP> &&parser) { parser.swap(this->parser); }

private:
  std::unique_ptr<DSP> parser;
};

class JSONToASTParser : public DomainSpecificParser {
    public:
        JSONToASTParser(const Json& json) : json{json} {}
    private:
        const Json json;
        // Implement these in a Top Down fashion
        virtual AST parseHelper() override;
        std::unique_ptr<ASTNode> parseRule(const Json&);
        std::unique_ptr<Rules> parseRules(const Json&);
        std::unique_ptr<FormatNode> parseFormatNode(const Json&);               
        std::unique_ptr<GlobalMessage> parseGlobalMessage(const Json&);         
        std::unique_ptr<VarDeclaration> parseVarDeclaration(const Json&);
        std::unique_ptr<Variable> parseVariable(const Json&);
        std::unique_ptr<ParallelFor> parseParallelFor(const Json&);
};

} // namespace AST

#endif
