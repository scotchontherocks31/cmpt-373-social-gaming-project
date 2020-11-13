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
  JSONToASTParser(Json &&json) : json{json} {}

private:
  const Json json;
  // Implement these in a Top Down fashion
  AST parseHelper() override;
  std::unique_ptr<ASTNode> parseRule(const Json &);
  std::unique_ptr<Rules> parseRules(const Json &);

  std::unique_ptr<FormatNode> parseFormatNode(const std::string &);
  std::unique_ptr<VarDeclaration> parseVarDeclaration(const std::string &);
  std::unique_ptr<Variable> parseVariable(const std::string &);
  std::unique_ptr<Condition> parseCondition(const std::string &);
  std::unique_ptr<Key> parseKey(const std::string &);
  std::unique_ptr<IntContainer> parseIntContainer(const int &);
  std::unique_ptr<AllSwitchCases> parseSwitchCases(const Json &);
  std::unique_ptr<AllWhenCases> parseWhenCases(const Json &);

  std::unique_ptr<Message> parseMessage(const Json &);
  std::unique_ptr<GlobalMessage> parseGlobalMessage(const Json &);
  std::unique_ptr<Scores> parseScores(const Json &);
  
  std::unique_ptr<ParallelFor> parseParallelFor(const Json &);
  std::unique_ptr<ForEach> parseForEach(const Json &);
  std::unique_ptr<Loop> parseLoop(const Json &);
  std::unique_ptr<InParallel> parseInParallel(const Json &);
  std::unique_ptr<Switch> parseSwitch(const Json &);
  std::unique_ptr<When> parseWhen(const Json &);

  std::unique_ptr<Reverse> parseReverse(const Json &);
  std::unique_ptr<Extend> parseExtend(const Json &);
  std::unique_ptr<Shuffle> parseShuffle(const Json &);
  std::unique_ptr<Sort> parseSort(const Json &);
  std::unique_ptr<Deal> parseDeal(const Json &);
  std::unique_ptr<Discard> parseDiscard(const Json &);

  std::unique_ptr<Add> parseAdd(const Json &);
  std::unique_ptr<Timer> parseTimer(const Json &);

  std::unique_ptr<InputChoice> parseInputChoice(const Json &);
  std::unique_ptr<InputText> parseInputText(const Json &);
  std::unique_ptr<InputVote> parseInputVote(const Json &);
};

} // namespace AST

#endif
