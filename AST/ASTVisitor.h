#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "ASTNode.h"
#include "DSLValue.h"
#include <algorithm>
#include <deque>
#include <iostream>
#include <json.hpp>
#include <map>
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
};

class Environment {
public:
  using Lexeme = std::string;

private:
  Environment *parent;
  std::unique_ptr<Environment> child;
  std::map<Lexeme, DSLValue> bindings;

public:
  Environment() : parent{nullptr} {}
  explicit Environment(Environment *parent) : parent{parent} {}
  DSLValue &getValue(const Lexeme &lexeme) noexcept { return bindings[lexeme]; }
  void removeBinding(const Lexeme &lexeme) noexcept {
    if (bindings.contains(lexeme)) {
      bindings.erase(lexeme);
    }
  }
  bool contains(const Lexeme &lexeme) noexcept {
    return bindings.contains(lexeme);
  }
  void setBinding(const Lexeme &lexeme, DSLValue value) noexcept {
    bindings.insert_or_assign(lexeme, std::move(value));
  }
  Environment &createChildEnvironment() noexcept {
    child = std::make_unique<Environment>(this);
    return *child;
  }
};

class ASTVisitor {
public:
  explicit ASTVisitor() = default;
  coro::Task<> visit(Root &node);
  coro::Task<> visit(Setup &node);
  coro::Task<> visit(GlobalMessage &node);
  coro::Task<> visit(FormatNode &node);
  coro::Task<> visit(Variable &node);
  coro::Task<> visit(VarDeclaration &node);
  coro::Task<> visit(Rules &node);
  coro::Task<> visit(ParallelFor &node);
  coro::Task<> visit(InputText &node);
  virtual ~ASTVisitor() = default;

private:
  virtual coro::Task<> visitHelper(Root &) = 0;
  virtual coro::Task<> visitHelper(Setup &) = 0;
  virtual coro::Task<> visitHelper(GlobalMessage &) = 0;
  virtual coro::Task<> visitHelper(FormatNode &) = 0;
  virtual coro::Task<> visitHelper(ParallelFor &) = 0;
  virtual coro::Task<> visitHelper(Rules &) = 0;
  virtual coro::Task<> visitHelper(Variable &) = 0;
  virtual coro::Task<> visitHelper(VarDeclaration &) = 0;
  virtual coro::Task<> visitHelper(InputText &) = 0;
};

// TODO: Add new visitors for new nodes : ParallelFor, Variable, VarDeclaration
// and Rules
class Interpreter : public ASTVisitor {
public:
  Interpreter(Environment &&env, Communicator &communicator)
      : environment{std::move(env)}, communicator{communicator} {}

private:
  coro::Task<> visitHelper(Root &node) override {
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
  }
  coro::Task<> visitHelper(Setup &node) override;
  coro::Task<> visitHelper(GlobalMessage &node) override {
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
  coro::Task<> visitHelper(InputText &node) override {
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
  coro::Task<> visitHelper(VarDeclaration &node) override {
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
  coro::Task<> visitHelper(ParallelFor &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  void visitEnter(GlobalMessage &node){};
  void visitLeave(GlobalMessage &node) {
    const auto &formatMessageNode = node.getFormatNode();
    auto &&formatMessage = formatMessageNode.getFormat();
    communicator.sendGlobalMessage(formatMessage);
  };

  void visitEnter(FormatNode &node){};
  void visitLeave(FormatNode &node){};

  void visitEnter(InputText &node){};
  void visitLeave(InputText &node){};

  void visitEnter(Variable &node){};
  void visitLeave(Variable &node){};

  void visitEnter(VarDeclaration &node){};
  void visitLeave(VarDeclaration &node){};

  coro::Task<> visitEnter(Rules &node) {

    for (auto &&child : node.getChildren()) {
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
    }
  };
  void visitLeave(Rules &node){};

  void visitEnter(ParallelFor &node){};
  void visitLeave(ParallelFor &node){};

private:
  Environment environment;
  Communicator &communicator;
};

// TODO: Add new visitors for new nodes : ParallelFor, Variable, VarDeclaration
// and Rules
class Printer : public ASTVisitor {
public:
  virtual ~Printer() { std::cout << "\n"; }
  Printer(std::ostream &out) : out{out} {}

private:
  coro::Task<> visitHelper(GlobalMessage &node) override {
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
  coro::Task<> visitHelper(InputText &node) override {
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
  coro::Task<> visitHelper(Variable &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(VarDeclaration &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
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
  void visitEnter(GlobalMessage &node) { out << "(GlobalMessage "; };
  void visitLeave(GlobalMessage &node) { out << ")"; };
  void visitEnter(FormatNode &node) {
    out << "(FormatNode \"" << node.getFormat() << "\" ";
  };
  void visitLeave(FormatNode &node) { out << ")"; };
  void visitEnter(InputText &node) { out << "(InputText "; };
  void visitLeave(InputText &node) { out << ")"; };
  void visitEnter(Rules &node) { out << "(Rules "; };
  void visitLeave(Rules &node) { out << ")"; };
  void visitEnter(Variable &node) {
    out << "(Variable \"" << node.getLexeme() << "\" ";
  };
  void visitLeave(Variable &node) { out << ")"; };
  void visitEnter(VarDeclaration &node) {
    out << "(VarDeclaration \"" << node.getLexeme() << "\" ";
  };
  void visitLeave(VarDeclaration &node) { out << ")"; };
  void visitEnter(ParallelFor &node) { out << "(ParallelFor "; };
  void visitLeave(ParallelFor &node) { out << ")"; };

private:
  std::ostream &out;
};

} // namespace AST
#endif
