#ifndef AST_NODE_H
#define AST_NODE_H

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <task.h>
#include <vector>
namespace AST {

class ASTVisitor;

class ASTNode {
public:
  int getChildrenCount() const { return this->numChildren; }
  std::vector<ASTNode *> getChildren() {
    std::vector<ASTNode *> returnValue;
    for (auto &x : children) {
      returnValue.push_back(x.get());
    }
    return returnValue;
  }
  const ASTNode &getParent() const { return *parent; }
  void setParent(ASTNode *parent) { parent = parent; }
  coro::Task<> accept(ASTVisitor &visitor) {
    auto coroutine = acceptHelper(visitor);
    while (not coroutine.isDone()) {
      co_await coroutine;
    }
  }
  virtual ~ASTNode(){};

protected:
  std::vector<std::unique_ptr<ASTNode>> children;
  ASTNode *parent;
  int numChildren;
  void appendChild(std::unique_ptr<ASTNode> &&child) {
    children.push_back(std::move(child));
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) = 0;
};

class FormatNode : public ASTNode { // parser complete
public:
  explicit FormatNode(std::string format) : format{std::move(format)} {}
  const std::string &getFormat() const { return format; }
  // {player.name}
  
private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
  std::string format;
};

class GlobalMessage : public ASTNode { // parser complete
public:
  explicit GlobalMessage(std::unique_ptr<FormatNode> &&formatNode) {
    appendChild(std::move(formatNode));
  }
  const FormatNode &getFormatNode() const {
    return *static_cast<FormatNode *>(children[0].get());
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Rules : public ASTNode {
public:
  explicit Rules() = default;
  void appendChild(std::unique_ptr<ASTNode> &&child) {
    ASTNode::appendChild(std::move(child));
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Variable : public ASTNode {
public:
  // TODO: Add types to check validity (eg. list vs bool)
  explicit Variable(std::string lexeme) : lexeme{std::move(lexeme)} {}
  const std::string &getLexeme() const { return lexeme; }
  //expression node for "to" : players
  //overload here

private:
  std::string lexeme;
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class VarDeclaration : public ASTNode {
public:
  explicit VarDeclaration(std::string lexeme) : lexeme{std::move(lexeme)} {}
  const std::string &getLexeme() const { return lexeme; }

private:
  std::string lexeme;
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class InputText : public ASTNode {
public:
  explicit InputText(std::unique_ptr<FormatNode> &&prompt,
                     std::unique_ptr<Variable> &&to,
                     std::unique_ptr<VarDeclaration> &&result) {
    appendChild(std::move(prompt));
    appendChild(std::move(to));
    appendChild(std::move(result));
  }
  const FormatNode &getPrompt() const {
    return *static_cast<FormatNode *>(children[0].get());
  }
  const Variable &getTo() const {
    return *static_cast<Variable *>(children[1].get());
  }
  const VarDeclaration &getResult() const {
    return *static_cast<VarDeclaration *>(children[2].get());
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class ParallelFor : public ASTNode {
public:
  ParallelFor(std::unique_ptr<Variable> &&variable,
              std::unique_ptr<VarDeclaration> &&varDeclaration,
              std::unique_ptr<Rules> &&rules) {
    appendChild(std::move(variable));
    appendChild(std::move(varDeclaration));
    appendChild(std::move(rules));
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};


//---
enum class OperatorType {
      DOT = 1,
      EQUALS
    };

const std::map<std::string , OperatorType> opMap =
  {
    {".", OperatorType::DOT},
    {"==", OperatorType::EQUALS}
  };
//---

class Operator : public ASTNode{
  public:
    Operator(std::string operatorString){
      operatorType = opMap(operatorString);
    }
    const OperatorType &getOperatorType(){ return operatorType; }
  private:
    OperatorType operatorType;

}

class UnaryOperation : public ASTNode{
  
  public:
    UnaryOperation(std::unique_ptr<Variable>&& operand,
                   std::unique_ptr<Operator>&& operator){
      appendChild(std::move(operand));
      appendChild(std::move(operator));
    }
    const Variable &getOperand() const {
      return *static_cast<Variable *>(children[0].get());
    }
    const Variable &getOperator() const {
      return *static_cast<Operator *>(children[1].get());
    }
  
}

//string cannot convert to operator, create a map that binds a string to an operator?
class BinaryOperation : public ASTNode{
  
  public:
    BinaryOperation(std::unique_ptr<Variable>&& operandLeft,
                    std::unique_ptr<Variable>&& operandRight,
                    std::unique_ptr<Operator>&& operator) {  
      appendChild(std::move(operandLeft));
      appendChild(std::move(operandRight));
      appendChild(std::move(operator));          
    }
     
    const Variable &getOperandLeft() const {
      return *static_cast<Variable *>(children[0].get());
    }
    const Variable &getOperandRight() const {
      return *static_cast<Variable *>(children[1].get());
    }
    const Operator &getOperator() const {
      return *static_cast<Operator *>(children[2].get());
    }
}


class AST {
public:
  AST(std::unique_ptr<ASTNode> &&root) : root{std::move(root)} {}
  const ASTNode &getParent() const { return *root; }
  void setRoot(std::unique_ptr<ASTNode> &&root) { root.swap(this->root); }
  coro::Task<> accept(ASTVisitor &visitor) {
    auto coroutine = root->accept(visitor);
    while (not coroutine.isDone()) {
      co_await coroutine;
    }
  }

private:
  std::unique_ptr<ASTNode> root;
};

} // namespace AST

#endif
