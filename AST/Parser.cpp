#include "Parser.h"
#include "ASTVisitor.h"
#include <assert.h>

namespace AST {

AST JSONToASTParser::parseHelper() {
  auto rootPtr = std::make_unique<Root>();
  rootPtr->appendChild(std::make_unique<Setup>());
  rootPtr->appendChild(parseRules(json[0]["rules"]));
  return AST{std::move(rootPtr)};
}

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

Environment Configurator::createEnvironment(
    std::vector<std::pair<int, std::string>> players) {
  auto config = json[0]["configuration"];
  auto enviro = Environment{nullptr};
  DSLValue setUp{config["setup"]};
  enviro.setBinding("configuration", setUp);

  // add the current members into the game
  auto perPlayer = json[0]["per-player"];
  Json aPlayer;
  for (auto i = players.begin(); i != players.end(); ++i) {
    aPlayer = {};
    aPlayer["id"] = i->first;
    aPlayer["name"] = i->second;
    for (Json::iterator it = perPlayer.begin(); it != perPlayer.end(); ++it) {
      aPlayer[it.key()] = it.value();
    }
    enviro.setBinding(i->second, DSLValue{aPlayer});
  }

  // add constants
  Json constants = json[0]["constants"];
  for (Json::iterator it = constants.begin(); it != constants.end(); ++it) {
    enviro.setBinding(it.key(), it.value());
  }

  // add variables
  Json variables = json[0]["variables"];
  for (Json::iterator it = variables.begin(); it != variables.end(); ++it) {
    enviro.setBinding(it.key(), it.value());
  }
  return enviro;
}

std::pair<int, int> Configurator::getPlayerCount() {
  auto config = json[0]["configuration"];
  auto playerMax = config["player count"]["max"];
  auto playerMin = config["player count"]["min"];
  return {playerMin, playerMax};
}

bool Configurator::hasAudience() {
  auto config = json[0]["configuration"];
  auto audience = config["audience"];
  return audience;
}

} // namespace AST
