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
  Environment createEnvironment(std::vector<std::pair<int,std::string>> players){  
  auto config = json[0]["configuration"];
  std::cout<<"NAME IS "<<config["name"]<<std::endl;
  
  //const std::map<userid, User *> &getMembers() const { return members; }
  
  auto enviro = Environment{nullptr};
  // int roundsInt = config["setup"]["Rounds"];
  // DSLValue rounds{roundsInt};
  // enviro.setBinding(std::string{"Rounds"}, rounds);

  DSLValue setUp{config["set-up"]};
  enviro.setBinding("configuration", setUp);

  
  // add the current members into the game
  // have vector<int,std::string> (id and name)
  /*
  std::vector<std::pair<int, std::string>> myVec (1, std::make_pair(0, "joe"));

   for (auto i = myVec.begin(); i != myVec.end(); ++i) 
        std::cout << i->first << " "; 
  */



  //add variables and constants
    // need to recursively create DSL values

  /*
  auto constants = json[0]["constants"];
  DSLValue weapons{constants[0]};
  enviro.setBinding("weapons",weapons );
  */



  // auto config = json[0]["variables"];
  // for variables
 /*
  std::cout<<json.dump(4)<<std::endl;
  auto constants = json[0]["constants"];
  auto weaponsJson = constants["weapons"];
  std::map<std::string,std::string> weapons;
  std::string name ;
  std::string beats;

  for(auto weapon:weaponsJson){
    std::cout<<weapon["name"]<<std::endl;
    name = weapon["name"];
    beats = weapon["beats"];
    weapons.insert({std::move(name),std::move(beats)});
  }
  std::cout<<"paper?"<<weapons["Scissors"]<<std::endl;
  */

  return enviro;
}


void parseConstants(const Json &constJson){
  // if map or list recursively parse
  // else return DSL; int, string, bool, ect
  // everthing needs to be DSL values?
  
}
std::pair<int,int> getPlayerCount(){
    auto config = json[0]["configuration"];
    auto playerMax = config["player count"]["max"];
    auto playerMin = config["player count"]["min"];
    return {playerMin,playerMax};
}


  private:
    const Json json;
};


} // namespace AST

#endif
