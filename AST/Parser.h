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
  std::unique_ptr<FormatNode> parseFormatNode(const Json &);
  std::unique_ptr<GlobalMessage> parseGlobalMessage(const Json &);
  std::unique_ptr<VarDeclaration> parseVarDeclaration(const Json &);
  std::unique_ptr<Variable> parseVariable(const Json &);
  std::unique_ptr<ParallelFor> parseParallelFor(const Json &);
};


class Configurator{
  public:
	Configurator(std::string json) : json{nlohmann::json::parse(std::move(json))}{
    //initialize the json

  }
  Environment createEnvironment(){  //(const Json &json) {


  auto config = json[0]["configuration"];
  std::cout<<"NAME IS "<<config["name"]<<std::endl;
  std::cout<<"rounds is "<<config["setup"]["Rounds"]<<std::endl;
  
  auto enviro = Environment{nullptr};
  int roundsInt = config["setup"]["Rounds"];
  DSLValue rounds{roundsInt};
  enviro.setBinding(std::string{"Rounds"}, rounds);
  return enviro;
}

  // Environment getEnvironment(){
  //   // parse json
  //   // create DSL
  //   // create env
  //   // insert dsl in env
  //   // return env
  // }
  private:
    const Json json;
    int playerMin;
    int playerMax;
};


} // namespace AST

#endif
