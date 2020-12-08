#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "ASTNode.h"
#include "ASTVisitor.h"
#include "json.hpp"
#include <memory>
using Json = nlohmann::json;

namespace AST {

enum RuleID {
  MESSAGE,
  GLOBAL_MESSAGE,
  SCORES,
  PARALLEL_FOR,
  FOR_EACH,
  LOOP,
  IN_PARALLEL,
  SWITCH,
  WHEN,
  REVERSE,
  EXTEND,
  SHUFFLE,
  SORT,
  DEAL,
  DISCARD,
  ADD,
  TIMER,
  INPUT_CHOICE,
  INPUT_TEXT,
  INPUT_VOTE
};

static std::map<std::string, RuleID> strToRules = {
    {"message", RuleID::MESSAGE},
    {"global-message", RuleID::GLOBAL_MESSAGE},
    {"scores", RuleID::SCORES},
    {"parallelfor", RuleID::PARALLEL_FOR},
    {"foreach", RuleID::FOR_EACH},
    {"loop", RuleID::LOOP},
    {"inparallel", RuleID::IN_PARALLEL},
    {"switch", RuleID::SWITCH},
    {"when", RuleID::WHEN},
    {"reverse", RuleID::REVERSE},
    {"extend", RuleID::EXTEND},
    {"shuffle", RuleID::SHUFFLE},
    {"sort", RuleID::SORT},
    {"deal", RuleID::DEAL},
    {"discard", RuleID::DISCARD},
    {"add", RuleID::ADD},
    {"timer", RuleID::TIMER},
    {"input-choice", RuleID::INPUT_CHOICE},
    {"input-text", RuleID::INPUT_TEXT},
    {"input-vote", RuleID::INPUT_VOTE}};

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

  static std::unique_ptr<ASTNode> parseRule(const Json &);
  static std::unique_ptr<Rules> parseRules(const Json &);

  static std::unique_ptr<ExpressionNode> parseExpression(const std::string &);
  static std::unique_ptr<FormatNode> parseFormatNode(const std::string &);
  static std::unique_ptr<Variable> parseVariable(const std::string &);
  static std::unique_ptr<AllSwitchCases> parseSwitchCases(const Json &);
  static std::unique_ptr<AllWhenCases> parseWhenCases(const Json &);

  static std::unique_ptr<Message> parseMessage(const Json &);
  static std::unique_ptr<GlobalMessage> parseGlobalMessage(const Json &);
  static std::unique_ptr<Scores> parseScores(const Json &);

  static std::unique_ptr<ParallelFor> parseParallelFor(const Json &);
  static std::unique_ptr<ForEach> parseForEach(const Json &);
  static std::unique_ptr<Loop> parseLoop(const Json &);
  static std::unique_ptr<InParallel> parseInParallel(const Json &);
  static std::unique_ptr<Switch> parseSwitch(const Json &);
  static std::unique_ptr<When> parseWhen(const Json &);

  static std::unique_ptr<Reverse> parseReverse(const Json &);
  static std::unique_ptr<Extend> parseExtend(const Json &);
  static std::unique_ptr<Shuffle> parseShuffle(const Json &);
  static std::unique_ptr<Sort> parseSort(const Json &);
  static std::unique_ptr<Deal> parseDeal(const Json &);
  static std::unique_ptr<Discard> parseDiscard(const Json &);

  static std::unique_ptr<Add> parseAdd(const Json &);
  static std::unique_ptr<Timer> parseTimer(const Json &);

  static std::unique_ptr<InputChoice> parseInputChoice(const Json &);
  static std::unique_ptr<InputText> parseInputText(const Json &);
  static std::unique_ptr<InputVote> parseInputVote(const Json &);

private:
  const Json json;
  // Implement these in a Top Down fashion
  AST parseHelper() override;
};

class ConfigParser {
public:
  ConfigParser(std::string json)
      : json{nlohmann::json::parse(std::move(json))} {}
  ConfigParser(Json json) : json{std::move(json)} {}
  static bool configJsonValid(const Json &json);
  std::string parseName();
  std::pair<size_t, size_t> parsePlayerCount();
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
