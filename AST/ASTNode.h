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
    while (not coroutine.isReady()) {
      co_await coroutine;
    }
  }
  virtual ~ASTNode(){};

protected:
  std::vector<std::unique_ptr<ASTNode>> children;
  ASTNode *parent;
  int numChildren;
  void appendChild(std::unique_ptr<ASTNode> &&child) { children.push_back(std::move(child)); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) = 0;
};

class FormatNode : public ASTNode {
public:
  explicit FormatNode(std::string format) : format{std::move(format)} {}
  const std::string &getFormat() const { return format; }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
  std::string format;
};

class GlobalMessage : public ASTNode {
public:
  explicit GlobalMessage(std::unique_ptr<FormatNode> &&formatNode) { appendChild(std::move(formatNode)); }
  const FormatNode &getFormatNode() const { return *static_cast<FormatNode *>(children[0].get()); }

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

class ParallelFor : public ASTNode {
public:
    ParallelFor(std::unique_ptr<Variable> &&variable, 
            std::unique_ptr<VarDeclaration> &&varDeclaration, std::unique_ptr<Rules> &&rules) {
        appendChild(std::move(variable));
        appendChild(std::move(varDeclaration));
        appendChild(std::move(rules));
    }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class AST {
public:
  AST(std::unique_ptr<ASTNode> &&root) : root{std::move(root)} {}
  const ASTNode &getParent() const { return *root; }
  void setRoot(std::unique_ptr<ASTNode> &&root) { root.swap(this->root); }
  coro::Task<> accept(ASTVisitor &visitor) {
    auto coroutine = root->accept(visitor);
    while (not coroutine.isReady()) {
      co_await coroutine;
    }
  }

private:
  std::unique_ptr<ASTNode> root;
};

} // namespace AST

#endif
