#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "ASTNode.h"
#include "ASTVisitor.h"
#include "json.hpp"
#include <memory>
using Json = nlohmann::json;

namespace AST {

class DomainSpecificParser {
public:
  AST parse() { return parseHelper(); }

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
  JSONToASTParser(std::string json)
      : json{nlohmann::json::parse(std::move(json))} {}
  JSONToASTParser(Json json) : json{std::move(json)} {}

private:
  const Json json;
  // Implement these in a Top Down fashion
  AST parseHelper() override;
  std::unique_ptr<ASTNode> parseRule(const Json &);
  std::unique_ptr<Rules> parseRules(const Json &);
  std::unique_ptr<FormatNode> parseFormatNode(const Json &);
  std::unique_ptr<GlobalMessage> parseGlobalMessage(const Json &);
  std::unique_ptr<VarDeclaration> parseVarDeclaration(const Json &);
  std::unique_ptr<Variable> parseVariable(const Json &);
  std::unique_ptr<ParallelFor> parseParallelFor(const Json &);
};

class ConfigParser {
public:
  ConfigParser(std::string json)
      : json{nlohmann::json::parse(std::move(json))} {}
  ConfigParser(Json json) : json{std::move(json)} {}
  std::string parseName();
  std::pair<int, int> parsePlayerCount();
  bool parseHasAudience();
  Json parseSetup();
  Json parsePerPlayer();
  Json parsePerAudience();
  Json parseVariables();
  Json parseConstants();

private:
  Json json;
};

struct CombinedParsers {
  ConfigParser configParser;
  JSONToASTParser astParser;
};

std::optional<CombinedParsers> generateParsers(std::string json);

} // namespace AST

#endif
