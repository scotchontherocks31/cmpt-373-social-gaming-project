#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "ASTNode.h"
#include <iostream>
#include <map>
#include <string>
#include <task.h>
#include <variant>

namespace AST {

class Communicator {
public:
  virtual void sendGlobalMessage(std::string message) = 0;
};

class PrintCommunicator : public Communicator {
public:
  void sendGlobalMessage(std::string message) override {
    std::cout << message << std::endl;
  }
};

class DSLValue;
using List = std::vector<DSLValue>;
using Map = std::map<std::string, DSLValue>;

template <typename T>
concept DSLType =
    std::is_convertible<T, bool>::value ||
    std::is_convertible<T, std::string>::value ||
    std::is_convertible<T, int>::value ||
    std::is_convertible<T, double>::value ||
    std::is_convertible<T, List>::value || std::is_convertible<T, Map>::value;

class DSLValue {
private:
  using InternalType =
      std::variant<std::monostate, bool, std::string, int, double, List, Map>;
  InternalType value;

public:
  template <DSLType T>
  DSLValue(T &&value) noexcept : value{std::forward<T>(value)} {}
  DSLValue() noexcept = default;
  DSLValue(const DSLValue &other) noexcept { this->value = other.value; }
  DSLValue(DSLValue &&other) noexcept { this->value = std::move(other.value); }
  template <DSLType T> T &get() { return std::get<T>(value); }
  template <DSLType T> auto &get_if() noexcept { return std::get_if<T>(value); }
  template <DSLType T> DSLValue &operator=(T &&a) noexcept {
    value = std::forward<T>(a);
    return *this;
  }
  DSLValue &operator=(const DSLValue &other) noexcept {
    this->value = other.value;
    return *this;
  }
  DSLValue &operator=(DSLValue &&other) noexcept {
    this->value = std::move(other.value);
    return *this;
  }
  DSLValue &operator[](const std::string &key) {
    Map &map = get<Map>();
    return map[key];
  }
  DSLValue &operator[](size_t index) {
    List &list = get<List>();
    return list[index];
  }
  List createKeyList(const std::string &key) {
    List returnList{};
    Map map = get<Map>();
    for (const auto &[x, y] : map) {
      returnList.push_back(y);
    }
    return returnList;
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
  coro::Task<> visit(GlobalMessage &node);
  coro::Task<> visit(FormatNode &node);
  coro::Task<> visit(Variable &node);
  coro::Task<> visit(VarDeclaration &node);
  coro::Task<> visit(Rules &node);
  coro::Task<> visit(ParallelFor &node);
  coro::Task<> visit(InputText &node);
  virtual ~ASTVisitor() = default;

private:
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
  coro::Task<> visitHelper(Rules &node) override {
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
  void visitEnter(GlobalMessage &node){};
  void visitLeave(GlobalMessage &node) {
    const auto &formatMessageNode = node.getFormatNode();
<<<<<<< HEAD
    const std::string &mess = formatMessageNode.getFormat();
    communication.sendGlobalMessage(mess);
=======
    auto &&formatMessage = formatMessageNode.getFormat();
    const std::string GAME_NAME = "Game Name";
    auto &&gameNameDSL = environment.getValue(GAME_NAME);
    auto &&gameName = gameNameDSL.get<std::string>();
    auto finalMessage = formatMessage + gameName;
    communicator.sendGlobalMessage(std::move(finalMessage));
>>>>>>> develop
  };

  void visitEnter(FormatNode &node){};
  void visitLeave(FormatNode &node){};

  void visitEnter(InputText &node){};
  void visitLeave(InputText &node){};

  void visitEnter(Variable &node){};
  void visitLeave(Variable &node){};

  void visitEnter(VarDeclaration &node){};
  void visitLeave(VarDeclaration &node){};

  void visitEnter(Rules &node){};
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
