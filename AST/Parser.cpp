#include "Parser.h"
#include "ASTVisitor.h"
#include <assert.h>
using Json = nlohmann::json;

namespace AST {

AST JSONToASTParser::parseHelper() { return AST{parseRules(json[0])}; }

std::unique_ptr<Rules> JSONToASTParser::parseRules(const Json &json) {

  auto rulePtr = std::make_unique<Rules>();
  for (const auto &rule : json) {
    auto &&x = parseRule(rule);
    rulePtr->appendChild(std::move(x));
  }
  return rulePtr;
}

std::unique_ptr<ASTNode> JSONToASTParser::parseRule(const Json &json) {
  if (json["rule"] == "global-message") {
    return parseGlobalMessage(json);
  } else if (json["rule"] == "parallelfor") {
    return parseParallelFor(json);
  } else {
    assert("rule not implemented yet");
    return std::unique_ptr<ASTNode>{};
  }
}

std::unique_ptr<FormatNode> JSONToASTParser::parseFormatNode(const Json &json) {

  return std::make_unique<FormatNode>(json["value"]);
}

std::unique_ptr<GlobalMessage>
JSONToASTParser::parseGlobalMessage(const Json &json) {

  auto &&format = parseFormatNode(json);
  return std::make_unique<GlobalMessage>(std::move(format));
}

std::unique_ptr<ParallelFor>
JSONToASTParser::parseParallelFor(const Json &json) {

  auto &&var = parseVariable(json);
  auto &&varDec = parseVarDeclaration(json);
  auto &&rules = parseRules(json["rules"]);

  return std::make_unique<ParallelFor>(std::move(var), std::move(varDec),
                                       std::move(rules));
}

std::unique_ptr<VarDeclaration>
JSONToASTParser::parseVarDeclaration(const Json &json) {

  return std::make_unique<VarDeclaration>(json["element"]);
}

std::unique_ptr<Variable> JSONToASTParser::parseVariable(const Json &json) {

  return std::make_unique<Variable>(json["list"]);
}

std::string ConfigParser::parseName() {
  auto &config = json[0]["configuration"];
  return config["name"].get<std::string>();
}

std::pair<int, int> ConfigParser::parsePlayerCount() {
  auto &config = json[0]["configuration"];
  auto playerMax = config["player count"]["max"].get<int>();
  auto playerMin = config["player count"]["min"].get<int>();
  return {playerMin, playerMax};
}

bool ConfigParser::parseHasAudience() {
  auto &config = json[0]["configuration"];
  return config["audience"].get<bool>();
}

Json ConfigParser::parseSetup() { return json[0]["configuration"]["setup"]; }

Json ConfigParser::parsePerPlayer() { return json[0]["per-player"]; }

Json ConfigParser::parsePerAudience() { return json[0]["per-audience"]; }

Json ConfigParser::parseVariables() { return json[0]["variables"]; }

Json ConfigParser::parseConstants() { return json[0]["constants"]; }

std::optional<CombinedParsers> generateParsers(std::string json) {
  if (!Json::accept(json)) {
    return {};
  }
  auto jsonObj = Json::parse(std::move(json));
  if (!jsonObj.contains("rules") || !jsonObj.contains("configuration")) {
    return {};
  }
  auto astParser = JSONToASTParser{std::move(jsonObj.at("rules"))};
  auto configParser = ConfigParser{std::move(jsonObj)};
  return CombinedParsers{std::move(configParser), std::move(astParser)};
}

} // namespace AST
