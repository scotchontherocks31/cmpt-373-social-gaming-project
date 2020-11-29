#include "ASTNode.h"
#include "ASTVisitor.h"
#include "CFGParser.h"

#include <task.h>

namespace AST {

enum class Terminal {
  COMMA,
  BIN,
  UN,
  OPENPAR,
  CLOSEPAR,
  ID,
  DOT,
  END,
};

static std::map<Type, Terminal> TypeToTerminal{
    // Anything that does not map is
    {Type::OPENPAR, Terminal::OPENPAR},   {Type::CLOSEPAR, Terminal::CLOSEPAR},
    {Type::DOT, Terminal::DOT},           {Type::EQUALS, Terminal::BIN},
    {Type::NOTEQUALS, Terminal::BIN},     {Type::GREATER, Terminal::BIN},
    {Type::GREATEREQUALS, Terminal::BIN}, {Type::LESS, Terminal::BIN},
    {Type::LESSEQUALS, Terminal::BIN},    {Type::NOT, Terminal::UN},
    {Type::COMMA, Terminal::COMMA},       {Type::ID, Terminal::ID}};

struct CFGExpressionWrapper {
  // I will use these Functions //
  CFGExpressionWrapper(std::vector<CFGExpression> tokens) {
    size = tokens.size();
    currentIndex = 0;
    for (auto t : tokens) {
      tokensQueue.push(t);
    }

    // Need a way for me to return Terminal::END at the very end
  }

  Terminal getTerminal() {
    if (currentIndex < size) {
      return TypeToTerminal[tokensQueue.front().getType()];
    } else {
      return Terminal::END;
    }

  } // If it is out of bounds, return Terminal::END.
  CFGExpression front() { return tokensQueue.front(); }
  void next_token() {
    tokensQueue.pop();
    currentIndex++;
  }
  std::string getValue() { return tokensQueue.front().getValue(); }
  Type getType() { return tokensQueue.front().getType(); }
  // ----------------------------
private:
  int size;
  int currentIndex;
  std::queue<CFGExpression> tokensQueue;
};

struct ExpressionASTParser {
  ExpressionASTParser(std::string str) : CFGTokens(parseToCFGExpression(str)) {}

  std::unique_ptr<ExpressionNode> parse_S() { // S -> E END_TOKEN
    auto &&result = empty_parse();
    while (CFGTokens.getTerminal() != Terminal::END) {
      result = parse_E();
    }
    return std::move(result);
  }

  std::unique_ptr<ExpressionNode> parse_E() { // E -> T E'
    auto &&result = parse_T();

    while (CFGTokens.getTerminal() == Terminal::BIN) { // E'-> BIN TE' | epsilon
      Type binaryOperator = CFGTokens.getType();
      CFGTokens.next_token();
      auto &&right = parse_T();
      result = std::make_unique<BinaryNode>(std::move(result), std::move(right),
                                            binaryOperator);
    }
    return std::move(result);
  }

  std::unique_ptr<ExpressionNode> parse_T() { // T -> UN T | F | (E)
    auto &&result = empty_parse();

    if (CFGTokens.getTerminal() == Terminal::UN) {
      auto unaryOperator = CFGTokens.getType();
      CFGTokens.next_token();

      auto &&operand = parse_T();
      result = std::make_unique<UnaryNode>(std::move(operand), unaryOperator);
    }

    if (CFGTokens.getTerminal() == Terminal::ID) {
      result = parse_F();
    }

    if (CFGTokens.getTerminal() == Terminal::OPENPAR) {
      CFGTokens.next_token();
      result = parse_E();
      CFGTokens.next_token();
    }

    return std::move(result);
  }

  std::unique_ptr<ExpressionNode> parse_F() { // F -> P F'
    auto &&result = parse_P();

    while (CFGTokens.getTerminal() == Terminal::DOT) { // F'-> DOT PF' | epsilon
      Type DOT = CFGTokens.getType();
      CFGTokens.next_token();

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

    if (CFGTokens.getTerminal() != Terminal::ID) { // epsilon
      return std::move(result);
    }

    else {
      result = std::make_unique<VariableExpression>(CFGTokens.getValue());
      CFGTokens.next_token();

      if (CFGTokens.getTerminal() == Terminal::OPENPAR) {
        CFGTokens.next_token();
        result = std::make_unique<FunctionCallNode>(std::move(result),
                                                    std::move(parse_arg()));
        CFGTokens.next_token(); // TODO: CHECK IF CLOSE PAR THEN THROW EXCEPTION
                           // INSTEAD
      }
    }

    return std::move(result);
  }

  std::vector<std::unique_ptr<ExpressionNode>> parse_arg() {

    std::vector<std::unique_ptr<ExpressionNode>> args;
    while (isE()) {
      args.push_back(parse_E());
      if (CFGTokens.getTerminal() == Terminal::COMMA) { // E, arglist
        CFGTokens.next_token();
      }
    }

    return std::move(args);
  }

  std::unique_ptr<ExpressionNode> empty_parse() {
    return std::make_unique<VariableExpression>("");
  }

  bool isE() { // E -> T -> UN T | F | (E)         F -> P -> ID
    if ((CFGTokens.getTerminal() == Terminal::UN) ||
        (CFGTokens.getTerminal() == Terminal::ID) ||
        (CFGTokens.getTerminal() == Terminal::OPENPAR)) {
      return true;
    } else {
      return false;
    }
  }

private:
  CFGExpressionWrapper CFGTokens;
};

} // namespace AST