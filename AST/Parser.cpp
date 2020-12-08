#include "Parser.h"
#include "ASTVisitor.h"
#include "CFGParser.h"
#include "ExpressionASTParser.h"
#include <algorithm>
#include <assert.h>
#include <exception>
#include <iostream>
#include <map>
#include <regex>
using Json = nlohmann::json;

namespace AST {

AST JSONToASTParser::parseHelper() { return AST{parseRules(json[0])}; }

std::unique_ptr<Rules> JSONToASTParser::parseRules(const Json &json) {

  auto rulePtr = std::make_unique<Rules>();

  if (json.empty())
    return rulePtr;

  for (const auto &rule : json) {
    auto &&x = parseRule(rule);
    rulePtr->appendChild(std::move(x));
  }
  return rulePtr;
}

std::unique_ptr<ASTNode> JSONToASTParser::parseRule(const Json &json) {

  auto itr = strToRules.find(json["rule"]);
  if (itr == strToRules.end())
    return std::unique_ptr<ASTNode>{};

  switch (itr->second) {
  case RuleID::MESSAGE:
    return parseMessage(json);
    break;
  case RuleID::GLOBAL_MESSAGE:
    return parseGlobalMessage(json);
    break;
  case RuleID::SCORES:
    return parseScores(json);
    break;
  case RuleID::PARALLEL_FOR:
    return parseParallelFor(json);
    break;
  case RuleID::FOR_EACH:
    return parseForEach(json);
    break;
  case RuleID::LOOP:
    return parseLoop(json);
    break;
  case RuleID::IN_PARALLEL:
    return parseInParallel(json);
    break;
  case RuleID::SWITCH:
    return parseSwitch(json);
    break;
  case RuleID::WHEN:
    return parseWhen(json);
    break;
  case RuleID::REVERSE:
    return parseReverse(json);
    break;
  case RuleID::EXTEND:
    return parseExtend(json);
    break;
  case RuleID::SHUFFLE:
    return parseShuffle(json);
    break;
  case RuleID::SORT:
    return parseSort(json);
    break;
  case RuleID::DEAL:
    return parseDeal(json);
    break;
  case RuleID::DISCARD:
    return parseDiscard(json);
    break;
  case RuleID::ADD:
    return parseAdd(json);
    break;
  case RuleID::TIMER:
    return parseTimer(json);
    break;
  case RuleID::INPUT_CHOICE:
    return parseInputChoice(json);
    break;
  case RuleID::INPUT_TEXT:
    return parseInputText(json);
    break;
  case RuleID::INPUT_VOTE:
    return parseInputVote(json);
    break;
  default:
    return std::unique_ptr<ASTNode>{};
  }
}

std::unique_ptr<FormatNode>
JSONToASTParser::parseFormatNode(const std::string &str) {
  std::regex argRegex{R"(\{.*\})"};
  std::vector<std::string> arguments;
  std::sregex_iterator reBegin{str.begin(), str.end(), argRegex};
  std::sregex_iterator reEnd{};
  for (auto it = reBegin; it != reEnd; ++it) {
    auto matchStr = it->str();
    assert(matchStr.size() > 2);
    arguments.push_back(matchStr.substr(1, matchStr.size() - 2));
  }
  auto formatStr = std::regex_replace(str, argRegex, "{}");
  std::vector<std::unique_ptr<ExpressionNode>> expressionNodes;
  expressionNodes.reserve(arguments.size());
  std::transform(
      arguments.begin(), arguments.end(), std::back_inserter(expressionNodes),
      [](auto &argStr) { return JSONToASTParser::parseExpression(argStr); });
  return std::make_unique<FormatNode>(std::move(formatStr),
                                      std::move(expressionNodes));
}

std::unique_ptr<Message> JSONToASTParser::parseMessage(const Json &json) {

  auto &&to = parseVariable(json["to"]);
  auto &&value = parseFormatNode(json["value"]);
  return std::make_unique<Message>(std::move(to), std::move(value));
}

std::unique_ptr<GlobalMessage>
JSONToASTParser::parseGlobalMessage(const Json &json) {

  auto &&format = parseFormatNode(json["value"]);
  return std::make_unique<GlobalMessage>(std::move(format));
}

std::unique_ptr<Scores> JSONToASTParser::parseScores(const Json &json) {

  auto &&score = parseVariable(json["score"]);
  auto ascending = json["ascending"];
  return std::make_unique<Scores>(std::move(score), ascending);
}

std::unique_ptr<ParallelFor>
JSONToASTParser::parseParallelFor(const Json &json) {

  auto &&var = parseVariable(json["list"]);
  auto &&varDec = parseVariable(json["element"]);
  auto &&rules = parseRules(json["rules"]);

  return std::make_unique<ParallelFor>(std::move(var), std::move(varDec),
                                       std::move(rules));
}

std::unique_ptr<Variable>
JSONToASTParser::parseVariable(const std::string &str) {

  return std::make_unique<Variable>(std::move(str));
}

std::unique_ptr<AllSwitchCases>
JSONToASTParser::parseSwitchCases(const Json &json) {

  auto casePtr = std::make_unique<AllSwitchCases>();
  if (json.empty())
    return casePtr;

  for (const auto &node : json) {
    auto &&value = parseFormatNode(node["case"]);
    auto &&rules = parseRules(node["rules"]);
    auto &&x = std::make_unique<SwitchCase>(std::move(value), std::move(rules));
    casePtr->appendChild(std::move(x));
  }
  return casePtr;
}

std::unique_ptr<AllWhenCases>
JSONToASTParser::parseWhenCases(const Json &json) {

  auto casePtr = std::make_unique<AllWhenCases>();
  if (json.empty())
    return casePtr;

  for (const auto &node : json) {
    auto &&cond = parseExpression(node["condition"]);
    auto &&rules = parseRules(node["rules"]);
    auto &&x = std::make_unique<WhenCase>(std::move(cond), std::move(rules));
    casePtr->appendChild(std::move(x));
  }
  return casePtr;
}

std::unique_ptr<ForEach> JSONToASTParser::parseForEach(const Json &json) {

  auto &&var = parseExpression(json["list"]);
  auto &&varDec = parseVariable(json["element"]);
  auto &&rules = parseRules(json["rules"]);

  return std::make_unique<ForEach>(std::move(var), std::move(varDec),
                                   std::move(rules));
}

std::unique_ptr<Loop> JSONToASTParser::parseLoop(const Json &json) {

  if (json.contains("until")) {
    auto &&cond = parseExpression(json["until"]);
    auto &&rules = parseRules(json["rules"]);

    return std::make_unique<Loop>(std::move(cond), std::move(rules));
  } else {
    auto &&cond = parseExpression(json["while"]);
    auto &&rules = parseRules(json["rules"]);

    return std::make_unique<Loop>(std::move(cond), std::move(rules));
  }
}

std::unique_ptr<InParallel> JSONToASTParser::parseInParallel(const Json &json) {
  // may be subject to change to allow coroutines to facilitate the array of
  // rules within
  auto &&rules = parseRules(json["rules"]);

  return std::make_unique<InParallel>(std::move(rules));
}

std::unique_ptr<Switch> JSONToASTParser::parseSwitch(const Json &json) {

  auto &&value = parseFormatNode(json["value"]);
  auto &&list = parseVariable(json["list"]);
  auto &&cases = parseSwitchCases(json["cases"]);

  return std::make_unique<Switch>(std::move(value), std::move(list),
                                  std::move(cases));
}

std::unique_ptr<When> JSONToASTParser::parseWhen(const Json &json) {

  auto &&cases = parseWhenCases(json["cases"]);

  return std::make_unique<When>(std::move(cases));
}

std::unique_ptr<Reverse> JSONToASTParser::parseReverse(const Json &json) {

  auto &&list = parseVariable(json["list"]);

  return std::make_unique<Reverse>(std::move(list));
}
std::unique_ptr<Extend> JSONToASTParser::parseExtend(const Json &json) {

  auto &&target = parseVariable(json["target"]);
  auto &&list = parseExpression(json["list"]);

  return std::make_unique<Extend>(std::move(target), std::move(list));
}
std::unique_ptr<Shuffle> JSONToASTParser::parseShuffle(const Json &json) {

  auto &&list = parseVariable(json["list"]);

  return std::make_unique<Shuffle>(std::move(list));
}
std::unique_ptr<Sort> JSONToASTParser::parseSort(const Json &json) {

  auto &&list = parseVariable(json["list"]);
  auto sortPointer = std::make_unique<Sort>(std::move(list));
  if (json.contains("key")) {
    auto &&key = parseExpression(json["key"]);
    sortPointer->addAttribute(std::move(key));
  }

  return sortPointer;
}
std::unique_ptr<Deal> JSONToASTParser::parseDeal(const Json &json) {

  auto &&from = parseVariable(json["from"]);
  auto &&to = parseVariable(json["to"]);
  auto &&count = parseExpression(json["count"]);

  return std::make_unique<Deal>(std::move(from), std::move(to),
                                std::move(count));
}
std::unique_ptr<Discard> JSONToASTParser::parseDiscard(const Json &json) {

  auto &&from = parseVariable(json["from"]);
  auto &&count = parseExpression(json["count"]);

  return std::make_unique<Discard>(std::move(from), std::move(count));
}

std::unique_ptr<Add> JSONToASTParser::parseAdd(const Json &json) {

  auto &&var = parseExpression(json["to"]);

  return std::make_unique<Add>(std::move(var), json["value"]);
}
std::unique_ptr<Timer> JSONToASTParser::parseTimer(const Json &json) {

  auto &&mode = parseVariable(json["mode"]);
  auto &&rules = parseRules(json["rules"]);
  auto timerPointer = std::make_unique<Timer>(json["duration"], std::move(mode),
                                              std::move(rules));

  if (json.contains("flag")) {
    auto &&flag = parseExpression(json["flag"]);
    timerPointer->addFlag(std::move(flag));
  }

  return timerPointer;
}

std::unique_ptr<InputChoice>
JSONToASTParser::parseInputChoice(const Json &json) {

  auto &&prompt = parseFormatNode(json["prompt"]);
  auto &&to = parseVariable(json["to"]);
  auto &&choices = parseExpression(json["choices"]);
  auto &&result = parseExpression(json["result"]);

  auto inputChoicePtr = std::make_unique<InputChoice>(
      std::move(prompt), std::move(to), std::move(choices), std::move(result));

  if (json.contains("timeout")) {
    inputChoicePtr->setTimeout(json["timeout"]);
  }
  return inputChoicePtr;
}
std::unique_ptr<InputText> JSONToASTParser::parseInputText(const Json &json) {

  auto &&prompt = parseFormatNode(json["prompt"]);
  auto &&to = parseVariable(json["to"]);
  auto &&result = parseExpression(json["result"]);

  auto inputTextPtr = std::make_unique<InputText>(
      std::move(prompt), std::move(to), std::move(result));

  if (json.contains("timeout")) {
    inputTextPtr->setTimeout(json["timeout"]);
  }
  return inputTextPtr;
}
std::unique_ptr<InputVote> JSONToASTParser::parseInputVote(const Json &json) {

  auto &&prompt = parseFormatNode(json["prompt"]);
  auto &&to = parseExpression(json["to"]);
  auto &&choices = parseExpression(json["choices"]);
  auto &&result = parseExpression(json["result"]);

  auto inputVotePtr = std::make_unique<InputVote>(
      std::move(prompt), std::move(to), std::move(choices), std::move(result));

  if (json.contains("timeout")) {
    inputVotePtr->setTimeout(json["timeout"]);
  }
  return inputVotePtr;
}

inline bool isUnsignedInteger(const Json &json) {
  return json.is_number_integer() and json.is_number_unsigned();
}

inline bool configHasValidName(const Json &config) {
  return config.contains("name") and config["name"].is_string();
}

inline bool configHasValidPlayerCount(const Json &config) {
  auto &playerCount = config["player count"];
  return playerCount.contains("max") and
         isUnsignedInteger(playerCount["max"]) and
         playerCount.contains("min") and isUnsignedInteger(playerCount["min"]);
}

inline bool configHasValidAudience(const Json &config) {
  return config.contains("audience") and config["audience"].is_boolean();
}

inline bool configHasValidSetup(const Json &config) {
  return config.contains("setup") and config["setup"].is_object();
}

std::string ConfigParser::parseName() {
  auto &config = json[0]["configuration"];
  if (configHasValidName(config)) {
    return config["name"].get<std::string>();
  }
  return {};
}

std::pair<size_t, size_t> ConfigParser::parsePlayerCount() {
  auto &config = json[0]["configuration"];
  if (configHasValidPlayerCount(config)) {
    auto &playerCount = config["player count"];
    auto playerMax = playerCount["max"].get<size_t>();
    auto playerMin = playerCount["min"].get<size_t>();
    return {playerMin, playerMax};
  }
  return {0, 0};
}

bool ConfigParser::parseHasAudience() {
  auto &config = json[0]["configuration"];
  if (configHasValidAudience(config)) {
    return config["audience"].get<bool>();
  }
  return false;
}

Json ConfigParser::parseSetup() { return json[0]["configuration"]["setup"]; }

Json ConfigParser::parsePerPlayer() { return json[0]["per-player"]; }

Json ConfigParser::parsePerAudience() { return json[0]["per-audience"]; }

Json ConfigParser::parseVariables() { return json[0]["variables"]; }

Json ConfigParser::parseConstants() { return json[0]["constants"]; }

bool ConfigParser::configJsonValid(const Json &json) {
  if (not json.contains("configuration") or not json.contains("constants") or
      not json.contains("variables") or not json.contains("per-player") or
      not json.contains("per-audience")) {
    return false;
  }
  auto &config = json["configuration"];
  if (not configHasValidName(config) or not configHasValidPlayerCount(config) or
      not configHasValidAudience(config) or not configHasValidSetup(config)) {
    return false;
  }
  return true;
}

std::optional<CombinedParsers> generateParsers(std::string json) {
  if (not Json::accept(json)) {
    return {};
  }
  auto jsonObj = Json::parse(std::move(json));
  if (not jsonObj.contains("rules") or not jsonObj.contains("configuration")) {
    return {};
  }
  if (not ConfigParser::configJsonValid(jsonObj)) {
    return {};
  }
  auto astParser = JSONToASTParser{std::move(jsonObj.at("rules"))};
  auto configParser = ConfigParser{std::move(jsonObj)};
  return CombinedParsers{std::move(configParser), std::move(astParser)};
}

std::unique_ptr<ExpressionNode>
JSONToASTParser::parseExpression(const std::string &str) {

  ExpressionASTParser expressionParse(str);
  return expressionParse.parse_S();
}

} // namespace AST
