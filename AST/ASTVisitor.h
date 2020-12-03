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
  coro::Task<> visit(Message &node);
  coro::Task<> visit(GlobalMessage &node);
  coro::Task<> visit(Scores &node);

  coro::Task<> visit(FormatNode &node);
  coro::Task<> visit(Variable &node);
  coro::Task<> visit(VarDeclaration &node);
  coro::Task<> visit(Condition &node);

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
  virtual ~ASTVisitor() = default;

private:
  virtual coro::Task<> visitHelper(Message &) = 0;
  virtual coro::Task<> visitHelper(GlobalMessage &) = 0;
  virtual coro::Task<> visitHelper(Scores &) = 0;

  virtual coro::Task<> visitHelper(FormatNode &) = 0;
  virtual coro::Task<> visitHelper(Variable &) = 0;
  virtual coro::Task<> visitHelper(VarDeclaration &) = 0;
  virtual coro::Task<> visitHelper(Condition &) = 0;

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
};

// TODO: Add new visitors for new nodes : ParallelFor, Variable, VarDeclaration
// and Rules
class Interpreter : public ASTVisitor {
public:
  Interpreter(Environment &&env, Communicator &communicator)
      : environment{std::move(env)}, communicator{communicator} {}

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
  coro::Task<> visitHelper(Condition &node) override {
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
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
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
  void visitEnter(VarDeclaration &node){};
  void visitLeave(VarDeclaration &node){};
  void visitEnter(Condition &node){};
  void visitLeave(Condition &node){};

  void visitEnter(Rules &node){};
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
  coro::Task<> visitHelper(VarDeclaration &node) override {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Condition &node) override {
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

  void visitEnter(Message &node) { out << "(Message "; };
  void visitLeave(Message &node) { out << ")"; };
  void visitEnter(GlobalMessage &node) { out << "(GlobalMessage "; };
  void visitLeave(GlobalMessage &node) { out << ")"; };
  void visitEnter(Scores &node) { out << "(Scores "; };
  void visitLeave(Scores &node) { out << ")"; };

  void visitEnter(FormatNode &node) {
    out << "(FormatNode \"" << node.getFormat() << "\" ";
  };
  void visitLeave(FormatNode &node) { out << ")"; };
  void visitEnter(Variable &node) {
    out << "(Variable \"" << node.getLexeme() << "\" ";
  };
  void visitLeave(Variable &node) { out << ")"; };
  void visitEnter(VarDeclaration &node) {
    out << "(VarDeclaration \"" << node.getLexeme() << "\" ";
  };
  void visitLeave(VarDeclaration &node) { out << ")"; };
  void visitEnter(Condition &node) {
    out << "(Condition \"" << node.getCond() << "\" ";
  };
  void visitLeave(Condition &node) { out << ")"; };

  void visitEnter(Rules &node) { out << "(Rules "; };
  void visitLeave(Rules &node) { out << ")"; };
  void visitEnter(AllSwitchCases &node) { out << "(AllSwitchCases "; };
  void visitLeave(AllSwitchCases &node) { out << ")"; };
  void visitEnter(AllWhenCases &node) { out << "(AllWhenCases "; };
  void visitLeave(AllWhenCases &node) { out << ")"; };
  void visitEnter(SwitchCase &node) { out << "(SwitchCase "; };
  void visitLeave(SwitchCase &node) { out << ")"; };
  void visitEnter(WhenCase &node) { out << "(WhenCase "; };
  void visitLeave(WhenCase &node) { out << ")"; };
  
  void visitEnter(ParallelFor &node) { out << "(ParallelFor "; };
  void visitLeave(ParallelFor &node) { out << ")"; };
  void visitEnter(ForEach &node) { out << "(ForEach "; };
  void visitLeave(ForEach &node) { out << ")"; };
  void visitEnter(Loop &node) { out << "(Loop "; };
  void visitLeave(Loop &node) { out << ")"; };
  void visitEnter(InParallel &node) { out << "(InParallel "; };
  void visitLeave(InParallel &node) { out << ")"; };
  void visitEnter(Switch &node) { out << "(Switch "; };
  void visitLeave(Switch &node) { out << ")"; };
  void visitEnter(When &node) { out << "(When "; };
  void visitLeave(When &node) { out << ")"; };

  void visitEnter(Reverse &node) { out << "(Reverse "; };
  void visitLeave(Reverse &node) { out << ")"; };
  void visitEnter(Extend &node) { out << "(Extend "; };
  void visitLeave(Extend &node) { out << ")"; };
  void visitEnter(Shuffle &node) { out << "(Shuffle "; };
  void visitLeave(Shuffle &node) { out << ")"; };
  void visitEnter(Sort &node) { out << "(Sort "; };
  void visitLeave(Sort &node) { out << ")"; };
  void visitEnter(Deal &node) { out << "(Deal "; };
  void visitLeave(Deal &node) { out << ")"; };
  void visitEnter(Discard &node) { out << "(Discard "; };
  void visitLeave(Discard &node) { out << ")"; };

  void visitEnter(Add &node) { out << "(Add "; };
  void visitLeave(Add &node) { out << ")"; };
  void visitEnter(Timer &node) { out << "(Timer "; };
  void visitLeave(Timer &node) { out << ")"; };

  void visitEnter(InputChoice &node) { out << "(InputChoice "; };
  void visitLeave(InputChoice &node) { out << ")"; };
  void visitEnter(InputText &node) { out << "(InputText "; };
  void visitLeave(InputText &node) { out << ")"; };
  void visitEnter(InputVote &node) { out << "(InputVote "; };
  void visitLeave(InputVote &node) { out << ")"; };

private:
  std::ostream &out;
};

} // namespace AST
#endif
