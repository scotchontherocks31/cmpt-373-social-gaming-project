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



class Interpreter : public ASTVisitor {
public:
  Interpreter(std::unique_ptr<Environment> &&env, Communicator &communicator)
      : environment{std::move(env)}, communicator{communicator} {}

  bool hasError() { return errorThrown; }
  std::unique_ptr<Environment> getEnvironment() {
    return std::move(environment);
  }

private:
  bool errorThrown = false;
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

  coro::Task<> visitHelper(BinaryNode &node) override {
    co_await visitEnter(node);
    visitLeave(node);
    co_return;
  }

  coro::Task<> visitHelper(UnaryNode &node) override {
    co_await visitEnter(node);
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
      // only visit right child if not dot. Else we only need the name of right child
      if(node.getBinaryOperator() != Type::DOT){
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
   
      case Type::DOT:
        {
          VariableExpression *rightChildVar =
          static_cast<VariableExpression *>(&rightChild);
          auto varName = rightChildVar->getLexeme();


          auto isList = typeCheck(valueLeft,DSLValue::Type::LIST);

          
          

          
          if (varName == "elements") { // return list as is
            if(!(typeCheck(valueLeft, DSLValue::Type::LIST))){
              errorThrown = true;
              co_return;

            }
            Symbol symbol = Symbol{valueLeft, false};
            env.allocateReturn(symbol);

          } else if (isList) { 
            auto weapons  = valueLeft.createSlice(varName);
            if(!(typeCheck(*weapons, DSLValue::Type::LIST))){
              errorThrown = true;
              co_return;

            }
           
            Symbol symbol = Symbol{*weapons, false};
            env.allocateReturn(symbol);
          } else { 
          if(!(typeCheck(valueLeft, DSLValue::Type::MAP))){
              errorThrown = true;
              co_return;

            }
            env.allocateReturn(Symbol{*valueLeft[varName], false});
          }
          
        }
        
        break;
      case Type::EQUALS: {
        
        if (!isSameType(valueRight, valueLeft) || !((typeCheck(valueLeft, DSLValue::Type::STRING)) || (typeCheck(valueLeft, DSLValue::Type::INT))||(typeCheck(valueLeft, DSLValue::Type::DOUBLE))||(typeCheck(valueLeft, DSLValue::Type::BOOLEAN)))) {
          errorThrown = true;
          co_return;
        }
  
        if(equal(valueLeft,valueRight)){
          Symbol symbol = *(equal(valueLeft,valueRight))? Symbol{DSLValue{true}, false}: Symbol{DSLValue{false}, false};
          env.allocateReturn(symbol);
        }
 
        
      } break;
      case Type::NOTEQUALS: {
        if (!isSameType(valueRight, valueLeft) || !((typeCheck(valueLeft, DSLValue::Type::STRING)) || (typeCheck(valueLeft, DSLValue::Type::INT))||(typeCheck(valueLeft, DSLValue::Type::DOUBLE))||(typeCheck(valueLeft, DSLValue::Type::BOOLEAN)))) {
          errorThrown = true;
          co_return;
        }

        if(equal(valueLeft,valueRight)){
          Symbol symbol = *(equal(valueLeft,valueRight))? Symbol{DSLValue{false}, false}: Symbol{DSLValue{true}, false};
          env.allocateReturn(symbol);
        }
      }
      
      break;
      case Type::GREATER: {

        if (!isSameType(valueRight, valueLeft) || !((typeCheck(valueLeft, DSLValue::Type::STRING)) || (typeCheck(valueLeft, DSLValue::Type::INT))||(typeCheck(valueLeft, DSLValue::Type::DOUBLE)))) {
          errorThrown = true;
          co_return;
        }

        if(greater(valueLeft,valueRight)){
          Symbol symbol = *(greater(valueLeft,valueRight))? Symbol{DSLValue{true}, false}: Symbol{DSLValue{false}, false};
          env.allocateReturn(symbol);
        }
      } break;
      case Type::LESS:
      {
        if (!isSameType(valueRight, valueLeft) || !((typeCheck(valueLeft, DSLValue::Type::STRING)) || (typeCheck(valueLeft, DSLValue::Type::INT))||(typeCheck(valueLeft, DSLValue::Type::DOUBLE)))) {
          errorThrown = true;
          co_return;
        }

        if(smaller(valueLeft,valueRight)){
          Symbol symbol = *(smaller(valueLeft,valueRight))? Symbol{DSLValue{true}, false}: Symbol{DSLValue{false}, false};
          env.allocateReturn(symbol);
        }
      }
        break;
      case Type::LESSEQUALS:
         {
        if (!isSameType(valueRight, valueLeft) || !((typeCheck(valueLeft, DSLValue::Type::STRING)) || (typeCheck(valueLeft, DSLValue::Type::INT))||(typeCheck(valueLeft, DSLValue::Type::DOUBLE)))) {
          errorThrown = true;
          co_return;
        }

        if(smaller(valueLeft,valueRight) && equal(valueLeft,valueRight) ){
          Symbol symbol = (*(smaller(valueLeft,valueRight)) || (*(equal(valueLeft,valueRight)))) ? Symbol{DSLValue{true}, false}: Symbol{DSLValue{false}, false};
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
      if(!typeCheck(value,DSLValue::Type::BOOLEAN)){
          errorThrown = true;
          co_return;
      }
      //notOperation(value);
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
  Communicator &communicator;
};



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
