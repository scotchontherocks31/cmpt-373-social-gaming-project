#include "Parser.h"
#include "ASTVisitor.h"
#include "ExpressionParser.h"
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

std::unique_ptr<ASTNode>
JSONToASTParser::parseExpression(const std::string &str) {
  struct RDP {
    RDP(std::string str) : safe(parseToType(str)) {}

    /*
    S -> E END_TOKEN
    E -> T E'
    E'-> BIN TE' | epsilon
    T -> UN T | F | (E)
    F -> P F'
    F'-> DOT PF' | epsilon
    P -> ID(arglist) | ID | epsilon
    arglist -> E
    arglist -> epsilon
    arglist -> E, arglist
    */

    std::unique_ptr<ASTNode> parse_S() { // S -> E END_TOKEN
      while (safe.getTerminal() != Terminal::END) {
        auto &&result = parse_E();
      }
    }

    std::unique_ptr<ASTNode> parse_E() { // E -> T E'
      auto &&result = parse_T();

      while (safe.getTerminal() == Terminal::BIN) { // E'-> BIN TE' | epsilon'
        safe.next_token();
        auto &&right = parse_T();
        // result = std::make_unique<Binary>(result, right , front().getType() ); //Create a Binary Node 
      }
      // return result;
    }

    std::unique_ptr<ASTNode> parse_T() {
      
    }

    std::unique_ptr<ASTNode> parse_F(){
      auto &&result = parse_P();

      while(safe.getTerminal() == Terminal::DOT){
        safe.next_token();
        auto &&right = parse_P();
        // result = std::make_unique<Binary>(result, right , front().getType() ); //Create a Binary Node of DOT
      }
      // return result;
    }

    //P -> ID(arglist) | ID | epsilon

    std::unique_ptr<ASTNode> parse_P(){
  
      auto && result = empty_parse();
      if (safe.getTerminal() != Terminal::ID){
        return result;
      }   
      else {
        safe.next_token();
        if(safe.getTerminal() == Terminal::OPENPAR){
          safe.next_token();
          result = std::make_unique<function>(functionName , parse_arg());
          safe.next_token();  // TODO: CHECK IF CLOSE PAR THEN THROW EXCEPTION INSTEAD
        }
      }
     
          
      return parse_variable();
    }

    std::unique_ptr<ASTNode> parse_arg(){

    }

    std::unique_ptr<ASTNode> parse_variable() {}
    std::unique_ptr<ASTNode> parse_function_call() {}
    std::unique_ptr<ASTNode> empty_parse() {}



  private:
    std::vector<TokenType> tokens;
    Safeway safe;
  };

  // auto tokens = parseToType(str);
  // return parseToNodes(tokens);
}

} // namespace AST
