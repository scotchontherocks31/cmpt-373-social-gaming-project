#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "ASTNode.h"
#include "DSLValue.h"
#include "Environment.h"
#include <algorithm>
#include <iostream>
#include <json.hpp>
#include <map>
#include <sstream>
#include <string>
#include <task.h>
#include <variant>
#include <list>
#include <deque> 

namespace AST {

using Json = nlohmann::json;

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
  Interpreter(std::unique_ptr<Environment> &&env, Communicator &communicator)
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
    co_await visitEnter(node);
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

  coro::Task<> visitEnter(ParallelFor &node){
   // auto &env = environment->createChildEnvironment(); 
   Environment env = environment->createChildEnvironment();

    // for (auto &&child : node.getChildren()) {
    //   auto task = child->accept(*this);
    //   while (not task.isDone()) {
    //     co_await task;
    //   }
    // }

    // set the mock players
  std::list<std::string> playersStrings {"Sarb","Ard","Vlad","Kabir","Tom","Jiho"}; // TODO: actually get this from env instead of mocking

  std::vector<DSLValue> playersDSL;
  for (auto &player: playersStrings) {

    playersDSL.push_back(DSLValue{player});
  }

  Symbol symbol = Symbol{playersDSL, false};
  Environment::Name key = "players";
  env.allocate(key, symbol);

  //std::cout<<"\nthis is list of playerst: "<<(env.find<false>(key)).value()<<std::endl;

  
  //auto handle = (env.find<false>(key)).value();

  std::deque<std::pair<coro::Task<>, DSLValue *>> tasks;

  for (auto &element : listVar) {
      tasks.push_back({(node.getRules()).accept(*this), &element});  // creating task with name of element that goes with it
  }



   

  /*
    coro::Task<> visitEnter(ParallelFor &node){
    auto &env = parentEnv->createChildEnvironment(); 
    auto &list = node.getListName();
    auto &element = node.getElementName();
    co_await list.accept(*this);  					// why do we need to co_await on a Variable Node
    co_await element.accept(*this);                 // why do we need to co_await on a VariableDeclarion Node
    env.setBinding(element.getLexeme(), {});       // set the first element to empty
    std::deque<std::pair<coro::Task<>, DSLValue *>> tasks;
    auto &listVar = *env.getVariable(list.getLexeme()); // list of DSL values
    
    for (auto &element : listVar) {
      tasks.push_back({(node.getRules()).accept(*this), &element});  // creating task with name of element that goes with it
    }
    std::deque<std::pair<coro::Task<>, DSLValue *>> waitingTasks;
    while (not tasks.empty()) {
      while (not tasks.empty()) {
        auto &task = tasks.front();
        tasks.pop_front();
        env.setVarBinding(element.getLexeme(), task.second);   // put element name inside the envi
        parentEnv = &env;
        task.first.resume();       // which environment will the task look into?
        if (not task.first.isDone()) {
          waitingTasks.push_back(std::move(task));
        }
      }
      if (not waitingTasks.empty()) {
        co_await std::suspend_always{};
      }
      std::ranges::move(waitingTasks, std::back_inserter(tasks));
    }
    co_return;
  };
  */
   co_return;
  };
  void visitLeave(ParallelFor &node){};

private:
  std::unique_ptr<Environment> environment;
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
  void visitEnter(GlobalMessage &node) { out << "(GlobalMessage"; };
  void visitLeave(GlobalMessage &node) { out << ")"; };
  void visitEnter(FormatNode &node) {
    out << "(FormatNode \"" << node.getFormat() << "\"";
  };
  void visitLeave(FormatNode &node) { out << ")"; };
  void visitEnter(InputText &node) { out << "(InputText"; };
  void visitLeave(InputText &node) { out << ")"; };
  void visitEnter(Rules &node) { out << "(Rules"; };
  void visitLeave(Rules &node) { out << ")"; };
  void visitEnter(Variable &node) {
    out << "(Variable\"" << node.getLexeme() << "\"";
  };
  void visitLeave(Variable &node) { out << ")"; };
  void visitEnter(VarDeclaration &node) {
    out << "(VarDeclaration\"" << node.getLexeme() << "\"";
  };
  void visitLeave(VarDeclaration &node) { out << ")"; };
  void visitEnter(ParallelFor &node) { out << "(ParallelFor"; };
  void visitLeave(ParallelFor &node) { out << ")"; };

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
