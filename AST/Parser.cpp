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

    std::unique_ptr<ExpressionNode> parse_S() { // S -> E END_TOKEN
      while (safe.getTerminal() != Terminal::END) {
        auto &&result = parse_E();
      }
    }

    std::unique_ptr<ExpressionNode> parse_E() { // E -> T E'
      auto &&result = parse_T();

      while (safe.getTerminal() == Terminal::BIN) { // E'-> BIN TE' | epsilon
        Type binaryOperator = safe.getType();
        safe.next_token();
        auto &&right = parse_T();
        result = std::make_unique<BinaryNode>( std::move(result) , std::move(right) , binaryOperator ); 
      }
      return std::move(result);
    }

    std::unique_ptr<ExpressionNode> parse_T() {  //T -> UN T | F | (E)
      auto &&result = empty_parse();

      if(safe.getTerminal() == Terminal::UN){ 
        auto unaryOperator = safe.getType();
        safe.next_token();
        
        auto &&operand = parse_T();
        result = std::make_unique<UnaryNode>( std::move(operand) , unaryOperator ); 
      }
       
      if(safe.getTerminal() == Terminal::ID){ 
        result = parse_F();
      }

      if(safe.getTerminal() == Terminal::OPENPAR){
        safe.next_token();
        result = parse_E();
        safe.next_token();
      }

      return std::move(result);
    }

    std::unique_ptr<ExpressionNode> parse_F(){ //F -> P F'
      auto &&result = parse_P();

      while(safe.getTerminal() == Terminal::DOT){ //F'-> DOT PF' | epsilon
        Type DOT = safe.getType();
        safe.next_token();

        auto &&right = parse_P();
        result = std::make_unique<BinaryNode>( std::move(result) , std::move(right) , DOT ); 
      }
      return std::move(result);
    }

    //------------------------------------
    //P -> ID(arglist) | ID | epsilon
    std::unique_ptr<ExpressionNode> parse_P(){
  
      auto && result = empty_parse();

      if (safe.getTerminal() != Terminal::ID){  //epsilon
        return std::move(result);
      }

      else {
        result = std::make_unique<VariableExpression>( safe.getValue() );   
        safe.next_token();

        if(safe.getTerminal() == Terminal::OPENPAR){    
          safe.next_token();
          result = std::make_unique<FunctionCallNode>( std::move(result) , std::move( parse_arg()) );
          safe.next_token();  // TODO: CHECK IF CLOSE PAR THEN THROW EXCEPTION INSTEAD
        }
      }
     
      return std::move(result);
    }

    std::vector<std::unique_ptr<ExpressionNode>> parse_arg() {

      std::vector<std::unique_ptr<ExpressionNode>> args;
      while(isE()){ 
        args.push_back( parse_E() );
        if( safe.getTerminal() == Terminal::COMMA){ // E, arglist
          safe.next_token();
        }
      }
      
      return std::move(args);
    }

    std::unique_ptr<ExpressionNode> empty_parse() {
      return std::make_unique<VariableExpression>("");
    }

    bool isE(){ // E -> T -> UN T | F | (E)         F -> P -> ID
      if( (safe.getTerminal() == Terminal::UN)  || 
          (safe.getTerminal() == Terminal::ID)  || 
          (safe.getTerminal() == Terminal::OPENPAR)){
              return true;
      }
    }

  private:
    std::vector<TokenType> tokens;
    Safeway safe;
  };

  // auto tokens = parseToType(str);
  // return parseToNodes(tokens);
}

} // namespace AST
