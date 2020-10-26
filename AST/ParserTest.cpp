#include "Parser.h"
#include "ASTNode.h"
#include "json.hpp"
#include <fstream>
using Json = nlohmann::json;

int main() {
  std::cout << "Starting test..." << std::endl;

  // create DSLValue
  AST::DSLValue dsl{std::string{"Greeting Game"}};
  // create enviroment
  AST::Environment enviro{nullptr};
  // insert DSL in environment
  enviro.setBinding(std::string{"Game Name"}, dsl);
  // create communication obj
  AST::Communication comm{};
  // create interpreter with enviroment and communication
  AST::Interpreter interp{std::move(enviro), comm};

  std::ifstream file("sample.json");
  Json rule = Json::parse(file);

  std::cout << "Got the JSON..." << std::endl;
  AST::JSONToASTParser JSONtoAST(std::move(rule)); // pass in JSON globalmessage
  AST::AST ast = JSONtoAST.parse();                // AST With GlobalMessage
  std::cout << "Got the JSON..." << std::endl;
  auto x = ast.accept(interp);
  x.resume();

  return 0;
}
