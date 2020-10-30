#include "Parser.h"
#include "ASTVisitor.h"
#include <assert.h>

namespace AST {

AST JSONToASTParser::parseHelper() { return AST{parseRules(json[0]["rules"])}; }

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

  //cat have multiple expression node for seperate expressions
  //maybe turn format string in to a vector?
  //e.g. format[0] = player.name
  //     format[1] = has won the game!
  //     format[2] = player.food
  //     format[3] = is the best!

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

std::unique_ptr<Operand> JSONToASTParser::parseOperand(std::string operand){
  //For now, just make it work with single binary operation in format node
  return std::make_unique<Operand>(operand);
}

std::unique_ptr<UnaryOperation> JSONToASTParser::parseUnaryOperation(std::string operator, std::string operandSingle){
  return std::make_unique<UnaryOperation>(operator, 
                                          std::move( std::make_unique<Operand>(operandSingle); ))
}

std::unique_ptr<BinaryOperation> JSONToASTParser::parseBinaryOperation(std::string operator, std::string operandLeft , std::string operandRight){
  return std::make_unique<UnaryOperation>(operator, 
                                          std::move( std::make_unique<Operand>(operandLeft), 
                                          std::move( std::make_unique<Operand>(operandRight); )
}

std::unique_ptr<Operation> JSONToASTParser::parseOperation(std::string operation){
  //parse the given operations
  std::vector<parsedOperation>

  //if one operand
  //return std::make_unique<UnaryOperation>(operator, operandSingle);

  //if two operand 
  return std::make_unique<BinaryOperation>(operator, operandLeft, operandRight);
}

std::unique_ptr<Expression> JSONToASTParser::parseExpression(std::string operations){
  // e.g. operations = player.name.food.drink.color
  // parse player.name, name.food , food.drink , drink.color in Operation node seperately in that order 
  // append the operation node as a child in that order (4 childs total in the example)

  // Ask about: Do we need multiple operation node inside expression node?
  // Because the return type can vary from string to list to, etc


  //for each operation...
  std::make_unique<Expression>( std::move( std::make_unique<Operation>(operation)) )
  
}
} // namespace AST
