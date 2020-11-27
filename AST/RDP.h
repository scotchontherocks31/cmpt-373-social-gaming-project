#include "ASTNode.h"
#include "ASTVisitor.h"
#include "ExpressionParser.h"

#include <task.h>

namespace AST {

struct RDP {
  RDP(std::string str) : safe(parseToType(str)) {}

  std::unique_ptr<ExpressionNode> parse_S() { // S -> E END_TOKEN
    auto &&result = empty_parse();
    while (safe.getTerminal() != Terminal::END) {
      result = parse_E();
    }
    return std::move(result);
  }

  std::unique_ptr<ExpressionNode> parse_E() { // E -> T E'
    auto &&result = parse_T();

    while (safe.getTerminal() == Terminal::BIN) { // E'-> BIN TE' | epsilon
      Type binaryOperator = safe.getType();
      safe.next_token();
      auto &&right = parse_T();
      result = std::make_unique<BinaryNode>(std::move(result), std::move(right),
                                            binaryOperator);
    }
    return std::move(result);
  }

  std::unique_ptr<ExpressionNode> parse_T() { // T -> UN T | F | (E)
    auto &&result = empty_parse();

    if (safe.getTerminal() == Terminal::UN) {
      auto unaryOperator = safe.getType();
      safe.next_token();

      auto &&operand = parse_T();
      result = std::make_unique<UnaryNode>(std::move(operand), unaryOperator);
    }

    if (safe.getTerminal() == Terminal::ID) {
      result = parse_F();
    }

    if (safe.getTerminal() == Terminal::OPENPAR) {
      safe.next_token();
      result = parse_E();
      safe.next_token();
    }

    return std::move(result);
  }

  std::unique_ptr<ExpressionNode> parse_F() { // F -> P F'
    auto &&result = parse_P();

    while (safe.getTerminal() == Terminal::DOT) { // F'-> DOT PF' | epsilon
      Type DOT = safe.getType();
      safe.next_token();

      auto &&right = parse_P();
      result = std::make_unique<BinaryNode>(std::move(result), std::move(right),
                                            DOT);
    }
    return std::move(result);
  }

  //------------------------------------
  // P -> ID(arglist) | ID | epsilon
  std::unique_ptr<ExpressionNode> parse_P() {

    auto &&result = empty_parse();

    if (safe.getTerminal() != Terminal::ID) { // epsilon
      return std::move(result);
    }

    else {
      result = std::make_unique<VariableExpression>(safe.getValue());
      safe.next_token();

      if (safe.getTerminal() == Terminal::OPENPAR) {
        safe.next_token();
        result = std::make_unique<FunctionCallNode>(std::move(result),
                                                    std::move(parse_arg()));
        safe.next_token(); // TODO: CHECK IF CLOSE PAR THEN THROW EXCEPTION
                           // INSTEAD
      }
    }

    return std::move(result);
  }

  std::vector<std::unique_ptr<ExpressionNode>> parse_arg() {

    std::vector<std::unique_ptr<ExpressionNode>> args;
    while (isE()) {
      args.push_back(parse_E());
      if (safe.getTerminal() == Terminal::COMMA) { // E, arglist
        safe.next_token();
      }
    }

    return std::move(args);
  }

  std::unique_ptr<ExpressionNode> empty_parse() {
    return std::make_unique<VariableExpression>("");
  }

  bool isE() { // E -> T -> UN T | F | (E)         F -> P -> ID
    if ((safe.getTerminal() == Terminal::UN) ||
        (safe.getTerminal() == Terminal::ID) ||
        (safe.getTerminal() == Terminal::OPENPAR)) {
      return true;
    } else {
      return false;
    }
  }

private:
  std::vector<TokenType> tokens;
  Safeway safe;
};

} // namespace AST