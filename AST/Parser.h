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


class Configurator{
  public:
	Configurator(std::string json) : json{nlohmann::json::parse(std::move(json))}{
    //initialize the json
  }
  Environment createEnvironment(auto players){  
  auto config = json[0]["configuration"];
  std::cout<<"NAME IS "<<config["name"]<<std::endl;
    
  auto enviro = Environment{nullptr};
  // int roundsInt = config["setup"]["Rounds"];
  // DSLValue rounds{roundsInt};
  // enviro.setBinding(std::string{"Rounds"}, rounds);

  //std::cout<<json.dump(4)<<std::endl;
  std::cout<<"okaay\n"<<(config["setup"]).dump(4)<<std::endl;
  DSLValue setUp{config["setup"]};
  enviro.setBinding("configuration", setUp);

  
  // add the current members into the game
  // have vector<int,std::string> (id and name)
  
  // std::vector<std::pair<int, std::string>> myVec (1, std::make_pair(0, "joe"));
   auto perPlayer = json[0]["per-player"];
   std::stringstream playerJson;
   Json playerJsonObj;
   for (auto i = players.begin(); i != players.end(); ++i){
      std::cout <<"next player id : "<< i->first <<"nammme is: "<<i->second<<" \n";
      playerJson.str("");
      playerJson << "{\"id\":"<<i->first<<",\"name\":\""<<i->second<<"\"";//,";
      for (Json::iterator it = perPlayer.begin(); it != perPlayer.end(); ++it) {
        playerJson << ",\""<<it.key()<<"\":"<<it.value()<<",";
      }
      playerJson.seekp(-1,playerJson.cur); playerJson << '}';
      auto playerJsonStr = playerJson.str();
      playerJsonObj = Json::parse(playerJsonStr);
      enviro.setBinding(i->second, DSLValue{playerJsonObj});
      

   }

    const std::string playerName = "106721347374288";
    auto &&playerDSL = enviro.getValue(playerName);
    //const Map &map
    std::cout<<"testing player:  "<<playerDSL<<std::endl;
    //std::cout<<"should be id 1:  "<<playerDSL["id"]<<std::endl;
    //auto &&playerMap = playerDSL.get<std::map>();
   
    //std::cout<<"testing player :  "<<playerMap["name"]<<std::endl;

      

  //add constants

  
  Json constants = json[0]["constants"];

  //std::cout<<constants.dump(4)<<std::endl;

  for (Json::iterator it = constants.begin(); it != constants.end(); ++it) {
    std::cout << it.key() << " : " << it.value() << "\n";
    enviro.setBinding(it.key(), it.value());
  }

  // testing constants
  const std::string weap = "weapons";
    auto &&weapDSL = enviro.getValue(weap);

    std::cout<<"testing weap:  "<<weapDSL<<std::endl;

  // add variables
  Json variables = json[0]["variables"];
  for (Json::iterator it = variables.begin(); it != variables.end(); ++it) {
    std::cout << it.key() << " : " << it.value() << "\n";
    enviro.setBinding(it.key(), it.value());
  }
  


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
