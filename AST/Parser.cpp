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

std::unique_ptr<ASTNode>
JSONToASTParser::parseExpression(const std::string &str) {

  ExpressionASTParser expressionParse(str);
  return expressionParse.parse_S();
}

} // namespace AST
