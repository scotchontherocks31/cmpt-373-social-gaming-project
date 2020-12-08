#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "ASTNode.h"
#include "CFGParser.h"
#include "DSLValue.h"
#include "Environment.h"
#include "Player.h"
#include <algorithm>
#include <deque>
#include <iostream>
#include <json.hpp>
#include <map>
#include <sstream>
#include <string>
#include <task.h>
#include <variant>

namespace AST {

using Json = nlohmann::json;

struct PlayerMessage {
  int playerId;
  std::string message;
};

class Communicator {
public:
  virtual void sendGlobalMessage(std::string message) = 0;
  virtual void sendToOwner(std::string message) = 0;
  virtual std::deque<PlayerMessage> receiveFromOwner() = 0;
};

class PrintCommunicator : public Communicator {
public:
  void sendGlobalMessage(std::string message) override {
    std::cout << message << std::endl;
  }
  void sendToOwner(std::string message) override {
    std::cout << message << std::endl;
  }
  std::deque<PlayerMessage> receiveFromOwner() override { return {}; };
};

class ASTVisitor {
public:
  explicit ASTVisitor() = default;
  coro::Task<> visit(Message &node);
  coro::Task<> visit(GlobalMessage &node);
  coro::Task<> visit(Scores &node);

  coro::Task<> visit(FormatNode &node);
  coro::Task<> visit(Variable &node);

  coro::Task<> visit(Rules &node);
  coro::Task<> visit(AllSwitchCases &node);
  coro::Task<> visit(AllWhenCases &node);
  coro::Task<> visit(SwitchCase &node);
  coro::Task<> visit(WhenCase &node);

  coro::Task<> visit(ParallelFor &node);
  coro::Task<> visit(ForEach &node);
  coro::Task<> visit(Loop &node);
  coro::Task<> visit(InParallel &node);
  coro::Task<> visit(Switch &node);
  coro::Task<> visit(When &node);

  coro::Task<> visit(Reverse &node);
  coro::Task<> visit(Extend &node);
  coro::Task<> visit(Shuffle &node);
  coro::Task<> visit(Sort &node);
  coro::Task<> visit(Deal &node);
  coro::Task<> visit(Discard &node);

  coro::Task<> visit(Add &node);
  coro::Task<> visit(Timer &node);

  coro::Task<> visit(InputChoice &node);
  coro::Task<> visit(InputText &node);
  coro::Task<> visit(InputVote &node);

  coro::Task<> visit(BinaryNode &node);
  coro::Task<> visit(UnaryNode &node);
  coro::Task<> visit(VariableExpression &node);
  coro::Task<> visit(FunctionCallNode &node);
  virtual ~ASTVisitor() = default;

private:
  virtual coro::Task<> visitHelper(Message &) = 0;
  virtual coro::Task<> visitHelper(GlobalMessage &) = 0;
  virtual coro::Task<> visitHelper(Scores &) = 0;

  virtual coro::Task<> visitHelper(FormatNode &) = 0;
  virtual coro::Task<> visitHelper(Variable &) = 0;

  virtual coro::Task<> visitHelper(Rules &) = 0;
  virtual coro::Task<> visitHelper(AllSwitchCases &) = 0;
  virtual coro::Task<> visitHelper(AllWhenCases &) = 0;
  virtual coro::Task<> visitHelper(SwitchCase &) = 0;
  virtual coro::Task<> visitHelper(WhenCase &) = 0;

  virtual coro::Task<> visitHelper(ParallelFor &) = 0;
  virtual coro::Task<> visitHelper(ForEach &) = 0;
  virtual coro::Task<> visitHelper(Loop &) = 0;
  virtual coro::Task<> visitHelper(InParallel &) = 0;
  virtual coro::Task<> visitHelper(Switch &) = 0;
  virtual coro::Task<> visitHelper(When &) = 0;

  virtual coro::Task<> visitHelper(Reverse &) = 0;
  virtual coro::Task<> visitHelper(Extend &) = 0;
  virtual coro::Task<> visitHelper(Shuffle &) = 0;
  virtual coro::Task<> visitHelper(Sort &) = 0;
  virtual coro::Task<> visitHelper(Deal &) = 0;
  virtual coro::Task<> visitHelper(Discard &) = 0;

  virtual coro::Task<> visitHelper(Add &) = 0;
  virtual coro::Task<> visitHelper(Timer &) = 0;

  virtual coro::Task<> visitHelper(InputChoice &) = 0;
  virtual coro::Task<> visitHelper(InputText &) = 0;
  virtual coro::Task<> visitHelper(InputVote &) = 0;

  virtual coro::Task<> visitHelper(BinaryNode &) = 0;
  virtual coro::Task<> visitHelper(UnaryNode &) = 0;
  virtual coro::Task<> visitHelper(VariableExpression &) = 0;
  virtual coro::Task<> visitHelper(FunctionCallNode &) = 0;
};

// TODO: Add new visitors for new nodes : ParallelFor, Variable & Rules
struct PopulatedEnvironment {
  std::unique_ptr<Environment> envPtr;
  PlayerList players;
};

class Interpreter : public ASTVisitor {
public:
  Interpreter(PopulatedEnvironment &&env, Communicator &communicator)
      : environment{std::move(env.envPtr)}, players{std::move(env.players)},
        communicator{communicator} {}

  bool hasError() { return errorThrown; }
  std::unique_ptr<Environment> getEnvironment() {
    return std::move(environment);
  }

private:
  bool errorThrown = false;

  coro::Task<> visitHelper(Message &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(GlobalMessage &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Scores &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(FormatNode &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Variable &node) final {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(Rules &node) override {
    co_await visitEnter(node);
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(AllSwitchCases &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(AllWhenCases &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(SwitchCase &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(WhenCase &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(ParallelFor &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(ForEach &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Loop &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(InParallel &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Switch &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(When &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(BinaryNode &node) override {
    co_await visitEnter(node);
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Reverse &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Extend &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Shuffle &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Sort &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Deal &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Discard &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Add &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Timer &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(UnaryNode &node) override {
    co_await visitEnter(node);
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(InputChoice &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(InputText &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(InputVote &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(VariableExpression &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(FunctionCallNode &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  void visitEnter(Message &node){};
  void visitLeave(Message &node){};
  void visitEnter(GlobalMessage &node){};
  void visitLeave(GlobalMessage &node) {
    const auto &formatMessageNode = node.getFormatNode();
    auto &&formatMessage = formatMessageNode.getFormat();
    communicator.sendGlobalMessage(formatMessage);
  };
  void visitEnter(Scores &node){};
  void visitLeave(Scores &node){};

  void visitEnter(FormatNode &node){};
  void visitLeave(FormatNode &node){};
  void visitEnter(Variable &node){};
  void visitLeave(Variable &node){};

  coro::Task<> visitEnter(Rules &node) {

    for (auto &&child : node.getChildren()) {
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
    }
  };
  void visitLeave(Rules &node){};
  void visitEnter(AllSwitchCases &node){};
  void visitLeave(AllSwitchCases &node){};
  void visitEnter(AllWhenCases &node){};
  void visitLeave(AllWhenCases &node){};
  void visitEnter(SwitchCase &node){};
  void visitLeave(SwitchCase &node){};
  void visitEnter(WhenCase &node){};
  void visitLeave(WhenCase &node){};

  void visitEnter(ParallelFor &node){};
  void visitLeave(ParallelFor &node){};
  void visitEnter(ForEach &node){};
  void visitLeave(ForEach &node){};
  void visitEnter(Loop &node){};
  void visitLeave(Loop &node){};
  void visitEnter(InParallel &node){};
  void visitLeave(InParallel &node){};
  void visitEnter(Switch &node){};
  void visitLeave(Switch &node){};
  void visitEnter(When &node){};
  void visitLeave(When &node){};

  void visitEnter(Reverse &node){};
  void visitLeave(Reverse &node){};
  void visitEnter(Extend &node){};
  void visitLeave(Extend &node){};
  void visitEnter(Shuffle &node){};
  void visitLeave(Shuffle &node){};
  void visitEnter(Sort &node){};
  void visitLeave(Sort &node){};
  void visitEnter(Deal &node){};
  void visitLeave(Deal &node){};
  void visitEnter(Discard &node){};
  void visitLeave(Discard &node){};

  void visitEnter(Add &node){};
  void visitLeave(Add &node){};
  void visitEnter(Timer &node){};
  void visitLeave(Timer &node){};

  void visitEnter(InputChoice &node){};
  void visitLeave(InputChoice &node){};
  void visitEnter(InputText &node){};
  void visitLeave(InputText &node){};
  void visitEnter(InputVote &node){};
  void visitLeave(InputVote &node){};

  coro::Task<> visitEnter(BinaryNode &node) {
    auto env = environment->createChildEnvironment();

    auto &leftChild = node.getArgOne();
    auto &rightChild = node.getArgTwo();
    coro::Task rightChildTask;
    DSLValue valueRight;

    // visit left child
    auto leftChildTask = leftChild.accept(*this);
    DSLValue valueLeft;
    while (not leftChildTask.isDone()) {
      co_await leftChildTask;
    }

    auto handleLeft = environment->getReturnValue();

    if (handleLeft) {
      valueLeft = *handleLeft;

    } else {

      errorThrown = true;
      co_return;
    }
    // only visit right child if not dot. Else we only need the name of right
    // child
    if (node.getBinaryOperator() != Type::DOT) {
      rightChildTask = rightChild.accept(*this);

      while (not rightChildTask.isDone()) {
        co_await rightChildTask;
      }

      auto handleRight = environment->getReturnValue();

      if (handleRight) {
        valueRight = *handleRight;

      } else {
        errorThrown = true;
        co_return;
      }
    }

    switch (node.getBinaryOperator()) {

    case Type::DOT: {
      VariableExpression *rightChildVar =
          static_cast<VariableExpression *>(&rightChild);
      auto varName = rightChildVar->getLexeme();

      auto isList = typeCheck(valueLeft, DSLValue::Type::LIST);

      if (varName == "elements") { // return list as is
        if (!(typeCheck(valueLeft, DSLValue::Type::LIST))) {
          errorThrown = true;
          co_return;
        }
        Symbol symbol = Symbol{valueLeft, false};
        env.allocateReturn(symbol);

      } else if (isList) {
        auto weapons = valueLeft.createSlice(varName);
        if (!(typeCheck(*weapons, DSLValue::Type::LIST))) {
          errorThrown = true;
          co_return;
        }

        Symbol symbol = Symbol{*weapons, false};
        env.allocateReturn(symbol);
      } else {
        if (!(typeCheck(valueLeft, DSLValue::Type::MAP))) {
          errorThrown = true;
          co_return;
        }
        env.allocateReturn(Symbol{*valueLeft[varName], false});
      }

    }

    break;
    case Type::EQUALS: {

      if (!isSameType(valueRight, valueLeft) ||
          !((typeCheck(valueLeft, DSLValue::Type::STRING)) ||
            (typeCheck(valueLeft, DSLValue::Type::INT)) ||
            (typeCheck(valueLeft, DSLValue::Type::DOUBLE)) ||
            (typeCheck(valueLeft, DSLValue::Type::BOOLEAN)))) {
        errorThrown = true;
        co_return;
      }

      if (equal(valueLeft, valueRight)) {
        Symbol symbol = *(equal(valueLeft, valueRight))
                            ? Symbol{DSLValue{true}, false}
                            : Symbol{DSLValue{false}, false};
        env.allocateReturn(symbol);
      }

    } break;
    case Type::NOTEQUALS: {
      if (!isSameType(valueRight, valueLeft) ||
          !((typeCheck(valueLeft, DSLValue::Type::STRING)) ||
            (typeCheck(valueLeft, DSLValue::Type::INT)) ||
            (typeCheck(valueLeft, DSLValue::Type::DOUBLE)) ||
            (typeCheck(valueLeft, DSLValue::Type::BOOLEAN)))) {
        errorThrown = true;
        co_return;
      }

      if (equal(valueLeft, valueRight)) {
        Symbol symbol = *(equal(valueLeft, valueRight))
                            ? Symbol{DSLValue{false}, false}
                            : Symbol{DSLValue{true}, false};
        env.allocateReturn(symbol);
      }
    }

    break;
    case Type::GREATER: {

      if (!isSameType(valueRight, valueLeft) ||
          !((typeCheck(valueLeft, DSLValue::Type::STRING)) ||
            (typeCheck(valueLeft, DSLValue::Type::INT)) ||
            (typeCheck(valueLeft, DSLValue::Type::DOUBLE)))) {
        errorThrown = true;
        co_return;
      }

      if (greater(valueLeft, valueRight)) {
        Symbol symbol = *(greater(valueLeft, valueRight))
                            ? Symbol{DSLValue{true}, false}
                            : Symbol{DSLValue{false}, false};
        env.allocateReturn(symbol);
      }
    } break;
    case Type::LESS: {
      if (!isSameType(valueRight, valueLeft) ||
          !((typeCheck(valueLeft, DSLValue::Type::STRING)) ||
            (typeCheck(valueLeft, DSLValue::Type::INT)) ||
            (typeCheck(valueLeft, DSLValue::Type::DOUBLE)))) {
        errorThrown = true;
        co_return;
      }

      if (smaller(valueLeft, valueRight)) {
        Symbol symbol = *(smaller(valueLeft, valueRight))
                            ? Symbol{DSLValue{true}, false}
                            : Symbol{DSLValue{false}, false};
        env.allocateReturn(symbol);
      }
    } break;
    case Type::LESSEQUALS: {
      if (!isSameType(valueRight, valueLeft) ||
          !((typeCheck(valueLeft, DSLValue::Type::STRING)) ||
            (typeCheck(valueLeft, DSLValue::Type::INT)) ||
            (typeCheck(valueLeft, DSLValue::Type::DOUBLE)))) {
        errorThrown = true;
        co_return;
      }

      if (smaller(valueLeft, valueRight) && equal(valueLeft, valueRight)) {
        Symbol symbol = (*(smaller(valueLeft, valueRight)) ||
                         (*(equal(valueLeft, valueRight))))
                            ? Symbol{DSLValue{true}, false}
                            : Symbol{DSLValue{false}, false};
        env.allocateReturn(symbol);
      }
    }

    break;

    default:
      std::cout << "default";
      // code block
    }
  };
  void visitLeave(BinaryNode &node){};

  coro::Task<> visitEnter(UnaryNode &node) {
    auto env = environment->createChildEnvironment();

    auto &child = node.getArgOne();

    switch (node.getUnaryOperator()) {
    case Type::NOT: {
      auto childTask = child.accept(*this);
      DSLValue value;
      while (not childTask.isDone()) {
        co_await childTask;
      }
      auto handle = environment->getReturnValue();

      if (handle) {
        value = *handle;
      } else {
        errorThrown = true;
        co_return;
      }
      if (!typeCheck(value, DSLValue::Type::BOOLEAN)) {
        errorThrown = true;
        co_return;
      }
      notOperation(value);
      Symbol symbol = Symbol{value, false};

      env.allocateReturn(symbol);

    } break;

    default:
      std::cout << "default";
      // code block
    }
  };
  void visitLeave(UnaryNode &node){};

  void visitEnter(VariableExpression &node) {

    auto env = environment->createChildEnvironment();

    Environment::Name key = node.getLexeme();
    auto handle = env.find(key);
    if (!handle) {
      errorThrown = true;
      return;
    }

    Symbol symbol = Symbol{*handle, false};

    env.allocateReturn(symbol);
  };
  void visitLeave(VariableExpression &node){};

  void visitEnter(FunctionCallNode &node){};
  void visitLeave(FunctionCallNode &node){};

private:
  std::unique_ptr<Environment> environment;
  PlayerList players;
  Communicator &communicator;
};

class Printer : public ASTVisitor {
public:
  virtual ~Printer() { std::cout << "\n"; }
  Printer(std::ostream &out) : out{out} {}

private:
  coro::Task<> visitHelper(Message &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(GlobalMessage &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Scores &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(FormatNode &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Variable &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(Rules &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(AllSwitchCases &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(AllWhenCases &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(SwitchCase &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(WhenCase &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(ParallelFor &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(ForEach &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Loop &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(InParallel &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Switch &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(When &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(Reverse &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Extend &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Shuffle &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Sort &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Deal &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Discard &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Add &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Timer &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(BinaryNode &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(UnaryNode &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(InputChoice &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(InputText &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(InputVote &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(VariableExpression &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(FunctionCallNode &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }

  void visitEnter(Message &node) { out << "(Message "; };
  void visitLeave(Message &node) { out << ")"; };
  void visitEnter(GlobalMessage &node) { out << "(GlobalMessage"; };
  void visitLeave(GlobalMessage &node) { out << ")"; };
  void visitEnter(Scores &node) { out << "(Scores "; };
  void visitLeave(Scores &node) { out << ")"; };

  void visitEnter(FormatNode &node) {
    out << "(FormatNode\"" << node.getFormat() << "\"";
  };
  void visitLeave(FormatNode &node) { out << ")"; };
  void visitEnter(Variable &node) {
    out << "(Variable\"" << node.getLexeme() << "\"";
  };
  void visitLeave(Variable &node) { out << ")"; };

  void visitEnter(Rules &node) { out << "(Rules"; };
  void visitLeave(Rules &node) { out << ")"; };
  void visitEnter(AllSwitchCases &node) { out << "(AllSwitchCases"; };
  void visitLeave(AllSwitchCases &node) { out << ")"; };
  void visitEnter(AllWhenCases &node) { out << "(AllWhenCases"; };
  void visitLeave(AllWhenCases &node) { out << ")"; };
  void visitEnter(SwitchCase &node) { out << "(SwitchCase"; };
  void visitLeave(SwitchCase &node) { out << ")"; };
  void visitEnter(WhenCase &node) { out << "(WhenCase"; };
  void visitLeave(WhenCase &node) { out << ")"; };

  void visitEnter(ParallelFor &node) { out << "(ParallelFor"; };
  void visitLeave(ParallelFor &node) { out << ")"; };
  void visitEnter(ForEach &node) { out << "(ForEach"; };
  void visitLeave(ForEach &node) { out << ")"; };
  void visitEnter(Loop &node) { out << "(Loop "; };
  void visitLeave(Loop &node) { out << ")"; };
  void visitEnter(InParallel &node) { out << "(InParallel"; };
  void visitLeave(InParallel &node) { out << ")"; };
  void visitEnter(Switch &node) { out << "(Switch"; };
  void visitLeave(Switch &node) { out << ")"; };
  void visitEnter(When &node) { out << "(When"; };
  void visitLeave(When &node) { out << ")"; };

  void visitEnter(Reverse &node) { out << "(Reverse"; };
  void visitLeave(Reverse &node) { out << ")"; };
  void visitEnter(Extend &node) { out << "(Extend"; };
  void visitLeave(Extend &node) { out << ")"; };
  void visitEnter(Shuffle &node) { out << "(Shuffle"; };
  void visitLeave(Shuffle &node) { out << ")"; };
  void visitEnter(Sort &node) { out << "(Sort"; };
  void visitLeave(Sort &node) { out << ")"; };
  void visitEnter(Deal &node) { out << "(Deal"; };
  void visitLeave(Deal &node) { out << ")"; };
  void visitEnter(Discard &node) { out << "(Discard"; };
  void visitLeave(Discard &node) { out << ")"; };

  void visitEnter(Add &node) { out << "(Add"; };
  void visitLeave(Add &node) { out << ")"; };
  void visitEnter(Timer &node) { out << "(Timer"; };
  void visitLeave(Timer &node) { out << ")"; };

  void visitEnter(InputChoice &node) { out << "(InputChoice"; };
  void visitLeave(InputChoice &node) { out << ")"; };
  void visitEnter(InputText &node) { out << "(InputText"; };
  void visitLeave(InputText &node) { out << ")"; };
  void visitEnter(InputVote &node) { out << "(InputVote"; };
  void visitLeave(InputVote &node) { out << ")"; };

  void visitEnter(BinaryNode &node) {
    out << "(BinaryNode:\"" << typeToString[node.getBinaryOperator()] << "\"";
  };
  void visitLeave(BinaryNode &node) { out << ")"; };

  void visitEnter(UnaryNode &node) {
    out << "(UnaryNode:\"" << typeToString[node.getUnaryOperator()] << "\"";
  };
  void visitLeave(UnaryNode &node) { out << ")"; };

  void visitEnter(VariableExpression &node) {
    out << "(VariableExpression\"" << node.getLexeme() << "\"";
  };
  void visitLeave(VariableExpression &node) { out << ")"; };

  void visitEnter(FunctionCallNode &node) {
    out << "(FunctionCallNode:\"" << (node.getFunctionName()).getLexeme()
        << "\"";
  };
  void visitLeave(FunctionCallNode &node) { out << ")"; };

private:
  std::ostream &out;

public:
  std::string returnOutput() {
    std::stringstream newStream;
    newStream << out.rdbuf();
    std::string myString = newStream.str();
    return myString;
  }
};

} // namespace AST
#endif
