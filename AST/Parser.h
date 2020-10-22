#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "ASTNode.h"
#include "ASTVisitor.h"
#include "json.hpp"
#include <memory>

using JSON = nlohmann::json;

namespace AST {

class DomainSpecificParser {
public:
  AST parse() { return parseHelper(); }
  virtual ~DomainSpecificParser();

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
  JSONToASTParser(JSON &&json) : json{std::move(json)} {}

private:
  JSON json;
  // Implement these in a Top Down fashion
  virtual AST parseHelper() override;
  FormatNode parseFormatNode();
  GlobalMessage parseGlobalMessage();
};

} // namespace AST

#endif
