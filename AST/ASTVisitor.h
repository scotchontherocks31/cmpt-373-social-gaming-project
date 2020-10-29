#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "ASTNode.h"
#include "Environment.h"
#include <iostream>
#include <map>
#include <string>
#include <task.h>
#include <variant>
#include <deque>
#include <json.hpp>

namespace AST {

class Communicator {
public:
  virtual void sendGlobalMessage(std::string message) = 0;
  virtual void sendPlayerMessage(size_t playerIndex, std::string message) = 0;
  virtual std::deque<std::string> getInputFromPlayer(size_t playerIndex) = 0;
};

class PrintCommunicator : public Communicator {
public:
  void sendGlobalMessage(std::string message) final {
    std::cout << message << std::endl;
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
      : globalEnv{std::move(env)}, parentEnv{&globalEnv}, communicator{communicator} {}

private:
  coro::Task<> visitHelper(GlobalMessage &node) final {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(FormatNode &node) final {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(InputText &node) final {
    visitEnter(node);
    auto &env = parentEnv->createChildEnvironment();
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    auto &prompt = node.getPrompt();
    auto &to = node.getTo();
    auto &toVar = env.getValue(to.getLexeme());
    communicator.sendPlayerMessage(toVar["index"] , prompt.getFormat());
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
  coro::Task<> visitHelper(VarDeclaration &node) final {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Rules &node) final {
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
  coro::Task<> visitHelper(ParallelFor &node) final {
    visitEnter(node);
    auto &env = parentEnv->createChildEnvironment();
    auto &list= node.getList();
    auto &element = node.getElement();
    co_await list.accept(*this);
    co_await element.accept(*this);
    env.setBinding(element.getLexeme(), {});
    std::deque<std::pair<coro::Task<>, std::string>> tasks;
    auto &listVar = env.getValue(list.getLexeme());
    for (auto &element : listVar) {
        tasks.push_back({(node.getRules()).accept(*this), element});
    }
    std::deque<std::pair<coro::Task<>, std::string>> waitingTasks;
    while (not tasks.empty()) {
      while (not tasks.empty()) {
          auto &task = tasks.front();
          tasks.pop_front();
          env.setBinding(element.getLexeme(), task.second);
          parentEnv = &env;
          task.first.resume();
          if (not task.first.isDone()) {
              waitingTasks.push_back(std::move(task));
          }
      }
      if (not waitingTasks.empty()) {
          co_await std::suspend_always{};
      }
      std::ranges::move(waitingTasks, std::back_inserter(tasks));
    }
    visitLeave(node);
    co_return;
  }
  void visitEnter(GlobalMessage &node){};
  void visitLeave(GlobalMessage &node) {
    const auto &formatMessageNode = node.getFormatNode();
    auto &&formatMessage = formatMessageNode.getFormat();
    communicator.sendGlobalMessage(std::move(formatMessage));
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
  Environment globalEnv;
  Environment *parentEnv;
  Communicator &communicator;
};

// TODO: Add new visitors for new nodes : ParallelFor, Variable, VarDeclaration
// and Rules
class Printer : public ASTVisitor {
public:
  virtual ~Printer() { std::cout << "\n"; }
  Printer(std::ostream &out) : out{out} {}

private:
  coro::Task<> visitHelper(GlobalMessage &node) final {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(FormatNode &node) final {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(InputText &node) final {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(Rules &node) final {
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
  coro::Task<> visitHelper(VarDeclaration &node) final {
    visitEnter(node);
    for (auto &&child : node.getChildren()) {
      co_await child->accept(*this);
    }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(ParallelFor &node) final {
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
