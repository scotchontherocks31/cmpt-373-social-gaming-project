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

  bool hasError() { return errorThrown; }

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

  coro::Task<> visitEnter(BinaryNode &node){
    auto env = environment->createChildEnvironment(); 
    std::cout << "(BinaryNode:\"" << typeToString[node.getBinaryOperator()] << "\"";
    auto &leftChild  = node.getArgOne();
    auto &rightChild = node.getArgTwo(); 


    switch(node.getBinaryOperator()) {
      case Type::CLOSEPAR:
        std::cout<<"this is closepar\n";
        // TODO
        break;
      case Type::ID:
        std::cout<<"this is id\n";
        // TODO
        break;
      case Type::DOT:
      {
        std::cout<<"this is dot\n";
        auto leftChildTask = leftChild.accept(*this);
        DSLValue valueLeft;
        while (not leftChildTask.isDone()) {
          co_await leftChildTask;
        }
        
        auto handleLeft = environment->getReturnValue();

        if(handleLeft){
              valueLeft = *handleLeft;
              std::cout<<"\nthe return value from left child is "<<valueLeft;

        }
        else{
          std::cout<<"handle did not work";
        }


        VariableExpression* rightChildVar = static_cast<VariableExpression*>(&rightChild);

        auto varName = rightChildVar->getLexeme();
        std::cout<<"\nwhat is the var name??"<<varName;

      
        // check to see if the left child is a list or map
        
        if(varName == "elements"){  // return list as is
            std::cout<<"\n Encountered elementsc, just return list as is";
            Symbol symbol = Symbol{valueLeft, false};
            env.allocateReturn(symbol);

        }
        else if(true){ // transform into new list
          std::cout<<"\n size is "<<valueLeft.size();
          std::vector<DSLValue> elements;
          //std::map<std::string, DSLValue>;

          for(int i = 0 ; i<valueLeft.size();i++){
            std::cout<<"\n elem: "<<*valueLeft[i];
          }

          std::vector<DSL> weaponNames= valueLeft.transform([varName](DSL aMap) -> DSL{aMap[varName]});
          valueLeft.transform()
        }
        else{ // not a list
          std::cout<<"\n and the name is: "<<*valueLeft[varName];
          Symbol symbol = Symbol{*valueLeft[varName], false};

          env.allocateReturn(symbol);
          

        }

        


      }
        break;
      case Type::EQUALS:
        {
        std::cout<<"this is equals\n";
        auto leftChildTask = leftChild.accept(*this);
        DSLValue valueLeft;
        while (not leftChildTask.isDone()) {
          co_await leftChildTask;
        }
        
        auto handleLeft = environment->getReturnValue();
      
        if(handleLeft){
              valueLeft = *handleLeft;
              std::cout<<"the return left value is "<<valueLeft;
        }
        else{
          std::cout<<"handle did not work";
        }

        std::cout<<"this is equals\n";
        auto rightChildTask = rightChild.accept(*this);
        DSLValue valueRight;
        while (not rightChildTask.isDone()) {
          co_await rightChildTask;
        }
        
        auto handleRight = environment->getReturnValue();
      
        if(handleRight){
              valueRight = *handleRight;
              std::cout<<"the return right value is "<<valueRight;
        }
        else{
          std::cout<<"handle did not work";
        }
        
        // do type check to make sure it is string, int, double, bool
        if(valueRight == valueLeft){
          std::cout<<"\n Yes they are equal!\n";
          Symbol symbol = Symbol{DSLValue{true}, false}; 
          env.allocateReturn(symbol);
        }
        else{
          std::cout<<"\n Yes they are not equal!\n";
          Symbol symbol = Symbol{DSLValue{false}, false}; 
          env.allocateReturn(symbol);
        }

   
        }
        break;
      case Type::NOTEQUALS:
        std::cout<<"this is NOTEQUALS\n";
        // TODO
        break;
      case Type::GREATER:
        std::cout<<"this is GREATER\n";
        // TODO
        break;
      case Type::LESS:
        std::cout<<"this is LESS\n";
        // TODO
        break;
      case Type::LESSEQUALS:
        std::cout<<"this is LESSEQUALS\n";
        // TODO
        break;
      case Type::COMMA:
        std::cout<<"this is COMMA\n";
        // TODO
        break;
      default:
          std::cout<<"default";
        // code block
    }

    

  };
  void visitLeave(BinaryNode &node){};

  coro::Task<> visitEnter(UnaryNode &node){
    auto env = environment->createChildEnvironment(); 
    std::cout << "(UnaryNode:\"" << typeToString[node.getUnaryOperator()] << "\"";
    auto &child  = node.getArgOne();


    switch(node.getUnaryOperator()) {
    case Type::NOT:
      {
        std::cout<<"this is NOT\n";
        // TODO
        auto childTask = child.accept(*this);
        DSLValue value;
        while (not childTask.isDone()) {
          co_await childTask;
        }
        
        auto handle = environment->getReturnValue();
      
        if(handle){
              value = *handle;
              std::cout<<"the return value is "<<value;
        }
        else{
          std::cout<<"handle did not work";
        }

          std::cout<<"\n in unary, allocating ! of "<<value;
          //Symbol symbol = Symbol{DSLValue{!value}, false}; 
          Symbol symbol = Symbol{DSLValue{false}, false}; 

          env.allocateReturn(symbol);

        
         }
        break;
     
     default:
          std::cout<<"default";
        // code block
    }
  };
  void visitLeave(UnaryNode &node){};

  void visitEnter(VariableExpression &node){

    auto env = environment->createChildEnvironment(); 
    std::cout << "(VariableExpression\"" << node.getLexeme() << "\"";
    // use find to find the variable
    
    Environment::Name key = node.getLexeme();
  auto handle = env.find(key);
  std::cout<<"\n found value: "<<*handle;

    Symbol symbol = Symbol{*handle, false};


    

    // set name of variable in allocate return
    env.allocateReturn(symbol);


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
