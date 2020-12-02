#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "ASTNode.h"
#include "CFGParser.h"
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
  coro::Task<> visit(BinaryNode &node);
  coro::Task<> visit(UnaryNode &node);
  coro::Task<> visit(VariableExpression &node);
  coro::Task<> visit(FunctionCallNode &node);
  virtual ~ASTVisitor() = default;

private:
  virtual coro::Task<> visitHelper(GlobalMessage &) = 0;
  virtual coro::Task<> visitHelper(FormatNode &) = 0;
  virtual coro::Task<> visitHelper(ParallelFor &) = 0;
  virtual coro::Task<> visitHelper(Rules &) = 0;
  virtual coro::Task<> visitHelper(Variable &) = 0;
  virtual coro::Task<> visitHelper(VarDeclaration &) = 0;
  virtual coro::Task<> visitHelper(InputText &) = 0;
  virtual coro::Task<> visitHelper(BinaryNode &) = 0;
  virtual coro::Task<> visitHelper(UnaryNode &) = 0;
  virtual coro::Task<> visitHelper(VariableExpression &) = 0;
  virtual coro::Task<> visitHelper(FunctionCallNode &) = 0;
};

// TODO: Add new visitors for new nodes : ParallelFor, Variable, VarDeclaration
// and Rules
class Interpreter : public ASTVisitor {
public:
  Interpreter(std::unique_ptr<Environment> &&env, Communicator &communicator)
      : environment{std::move(env)}, communicator{communicator} {}

private:
  coro::Task<> visitHelper(GlobalMessage &node) override {
    co_await visitEnter(node);
    // for (auto &&child : node.getChildren()) {
    //   co_await child->accept(*this);
    // }
    visitLeave(node);
    co_return;
  }
  coro::Task<> visitHelper(FormatNode &node) override {
    co_await visitEnter(node);
    // for (auto &&child : node.getChildren()) {
    //   co_await child->accept(*this);
    // }
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

  coro::Task<>  visitEnter(GlobalMessage &node){
     auto &formatMessageNode = node.getFormatNode(); 
    //auto &&formatMessage = formatMessageNode.getFormat(); // delete

    auto task = formatMessageNode.accept(*this);  // accept on it's child is already being called by its parent
    while (not task.isDone()) {
      co_await task;
    }

    // call accept on format node
    // get back the formatted string and send to communicator, how do I do this?
    communicator.sendGlobalMessage("implementing expressions in format node");
  };
  void visitLeave(GlobalMessage &node) {};

  coro::Task<> visitEnter(FormatNode &node){
    auto &&formatMessage = node.getFormat();
    std::cout<<"the formatnode visitor is being called"<<std::endl;
    // call visit on each child
  
    for (auto &&child : node.getChildren()) {
      auto task = child->accept(*this);
      while (not task.isDone()) {
        co_await task;
      }
      // collect the results  in vector
    }
      
    
    // replace the placeholders in the format node one by one
  };
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

  void visitEnter(BinaryNode &node){
    // this doenst have to be concurrent and use coawait?
    std::cout<<"entering binary node"<<std::endl;
    // switch
    //get left expression
    // call accept on left expressoin
    // retreive the result of the visit from the environment

    // get right expression
    // call accept
    // retreive the result from the environment

    // perform the binary operator the left variable with right variable
      // if right doesnt exist in left

    // put inside the environment for the parent to use
  };
  void visitLeave(BinaryNode &node){};

  void visitEnter(UnaryNode &node){};
  void visitLeave(UnaryNode &node){};

  void visitEnter(VariableExpression &node){
    std::cout<<"entering variabel expression"<<std::endl;
    // put the variable into the environment for parent to use
  };
  void visitLeave(VariableExpression &node){};

  void visitEnter(FunctionCallNode &node){};
  void visitLeave(FunctionCallNode &node){};

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

  void visitEnter(GlobalMessage &node) { out << "(GlobalMessage"; };
  void visitLeave(GlobalMessage &node) { out << ")"; };
  void visitEnter(FormatNode &node) {
    out << "(FormatNode\"" << node.getFormat() << "\"";
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
