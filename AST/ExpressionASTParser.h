#include "ASTNode.h"
#include "ASTVisitor.h"
#include "CFGParser.h"
#include <assert.h>
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
  CFGExpressionWrapper(std::vector<CFGExpression> tokens){
    
    for (auto t : tokens) {
      tokensQueue.push(t);
    }
  }

  Terminal getTerminal() {
    if (!tokensQueue.empty()) {
      return TypeToTerminal[tokensQueue.front().getType()];
    } else {
      return Terminal::END;
    }
  }
  CFGExpression front() { return tokensQueue.front(); }
  void next_token() {
    tokensQueue.pop();

  }
  std::string getValue() { return tokensQueue.front().getValue(); }
  Type getType() { return tokensQueue.front().getType(); }
  // ----------------------------
private:
  std::queue<CFGExpression> tokensQueue;
};

struct ExpressionASTParser {
  ExpressionASTParser(std::string str) : CFGTokens(parseToCFGExpression(str)) {}

  std::unique_ptr<ExpressionNode> parse_S() { // S -> E END_TOKEN
    std::unique_ptr<ExpressionNode> result = empty_parse();
    while (CFGTokens.getTerminal() != Terminal::END) {
      result = parse_E();
    }
    return std::move(result);
  }

  std::unique_ptr<ExpressionNode> parse_E() { // E -> T E'
    std::unique_ptr<ExpressionNode> result = parse_T();

    while (CFGTokens.getTerminal() == Terminal::BIN) { // E'-> BIN TE' | epsilon
      Type binaryOperator = CFGTokens.getType();
      CFGTokens.next_token();
      assert(isT());
      auto &&right = parse_T();
      result = std::make_unique<BinaryNode>(std::move(result), std::move(right),
                                            binaryOperator);
    }
    return std::move(result);
  }

  std::unique_ptr<ExpressionNode> parse_T() { // T -> UN T | F | (E)
    std::unique_ptr<ExpressionNode> result = empty_parse();

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
      assert(CFGTokens.getTerminal() == Terminal::CLOSEPAR);
      CFGTokens.next_token();
    }

    return std::move(result);
  }

  std::unique_ptr<ExpressionNode> parse_F() { // F -> P F'
    std::unique_ptr<ExpressionNode> result = parse_P();

    while (CFGTokens.getTerminal() == Terminal::DOT) { // F'-> DOT PF' | epsilon
      Type DOT = CFGTokens.getType();
      CFGTokens.next_token();
      assert(  (isP() , "ID Expected for the next token") );
      auto &&right = parse_P();
      result = std::make_unique<BinaryNode>(std::move(result), std::move(right),
                                            DOT);
    }
    return std::move(result);
  }

  //------------------------------------
  // P -> ID(arglist) | ID | epsilon
  std::unique_ptr<ExpressionNode> parse_P() {

    std::unique_ptr<ExpressionNode> result = empty_parse();

    if (CFGTokens.getTerminal() != Terminal::ID) { // epsilon
      return std::move(result);
    }

    else if (CFGTokens.getTerminal() == Terminal::ID) {
      result = std::make_unique<VariableExpression>(CFGTokens.getValue());
      CFGTokens.next_token();

      if (CFGTokens.getTerminal() == Terminal::OPENPAR) {
        CFGTokens.next_token();
        result = std::make_unique<FunctionCallNode>(std::move(result),
                                                    std::move(parse_arg()));

        assert(CFGTokens.getTerminal() == Terminal::CLOSEPAR);
        CFGTokens.next_token();
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
        assert(isE()); // if there is a comma, we expect another E
      }
    }

    return std::move(args);
  }

  std::unique_ptr<ExpressionNode> empty_parse() {
    return std::make_unique<VariableExpression>("");
  }

  // E T F P arg
  bool isT() { // E -> T -> UN T | F | (E)         F -> P -> ID
    if ((CFGTokens.getTerminal() == Terminal::UN) ||
        (CFGTokens.getTerminal() == Terminal::ID) ||
        (CFGTokens.getTerminal() == Terminal::OPENPAR)) {
      return true;
    } else {
      return false;
    }
  }
  bool isE() { return isT(); }
  bool isP() { return (CFGTokens.getTerminal() == Terminal::ID); }

private:
  CFGExpressionWrapper CFGTokens;
};

} // namespace AST