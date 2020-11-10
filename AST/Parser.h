#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "ASTNode.h"
#include "ASTVisitor.h"
#include "json.hpp"
#include <memory>
#include <sstream>
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
  std::unique_ptr<FormatNode> parseFormatNode(const Json &);
  std::unique_ptr<GlobalMessage> parseGlobalMessage(const Json &);
  std::unique_ptr<VarDeclaration> parseVarDeclaration(const Json &);
  std::unique_ptr<Variable> parseVariable(const Json &);
  std::unique_ptr<ParallelFor> parseParallelFor(const Json &);
};

class Configurator {
public:
  Configurator(std::string json)
      : json{nlohmann::json::parse(std::move(json))} {}
  Environment createEnvironment(auto players) {
    auto config = json[0]["configuration"];
    auto enviro = Environment{nullptr};
    DSLValue setUp{config["setup"]};
    enviro.setBinding("configuration", setUp);

    // add the current members into the game
    auto perPlayer = json[0]["per-player"];
    std::stringstream playerJson;
    Json playerJsonObj;
    for (auto i = players.begin(); i != players.end(); ++i) {
      playerJson.str("");
      playerJson << "{\"id\":" << i->first << ",\"name\":\"" << i->second
                 << "\""; //,";
      for (Json::iterator it = perPlayer.begin(); it != perPlayer.end(); ++it) {
        playerJson << ",\"" << it.key() << "\":" << it.value() << ",";
      }
      playerJson.seekp(-1, playerJson.cur);
      playerJson << '}';
      playerJsonObj = Json::parse(playerJson.str());
      enviro.setBinding(i->second, DSLValue{playerJsonObj});
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

  std::pair<int, int> getPlayerCount() {
    auto config = json[0]["configuration"];
    auto playerMax = config["player count"]["max"];
    auto playerMin = config["player count"]["min"];
    return {playerMin, playerMax};
  }

  bool hasAudience() {
    auto config = json[0]["configuration"];
    auto audience = config["audience"];
    return audience;
  }

private:
  const Json json;
};

} // namespace AST

#endif
