#ifndef AST_NODE_H
#define AST_NODE_H

#include "Environment.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <task.h>
#include <vector>

namespace AST {

namespace {

template <typename T> T &cast(auto &children, int index) {
  return *static_cast<T *>(children[index].get());
}

} // namespace

class ASTVisitor;

class ASTNode {
public:
  explicit ASTNode() = default;
  explicit ASTNode(size_t size) : children{size} {}
  int getChildrenCount() const { return children.size(); }
  std::vector<ASTNode *> getChildren() {
    std::vector<ASTNode *> returnValue;
    for (auto &x : children) {
      returnValue.push_back(x.get());
    }
    return returnValue;
  }
  const ASTNode &getParent() const { return *parent; }
  void setParent(ASTNode *parent) { this->parent = parent; }
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
    return cast<FormatNode>(children, 0);
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Rules : public ASTNode {
public:
  explicit Rules(auto &&nodes) : ASTNode{nodes.size()} {
    std::ranges::for_each(nodes,
                          [this](auto &&node) { node->setParent(this); });
    std::ranges::move(nodes, children.begin());
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

class InputText : public ASTNode {
public:
  explicit InputText(std::unique_ptr<FormatNode> &&prompt,
                     std::unique_ptr<Variable> &&to,
                     std::unique_ptr<VarDeclaration> &&result) {
    appendChild(std::move(prompt));
    appendChild(std::move(to));
    appendChild(std::move(result));
  }
  const FormatNode &getPrompt() const { return cast<FormatNode>(children, 0); }
  const Variable &getTo() const { return cast<Variable>(children, 1); }
  const VarDeclaration &getResult() const {
    return cast<VarDeclaration>(children, 2);
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
  const Variable &getList() const { return cast<Variable>(children, 0); }
  const VarDeclaration &getElement() const {
    return cast<VarDeclaration>(children, 1);
  }
  const Rules &getRules() const { return cast<Rules>(children, 2); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class AST {
public:
  AST(std::unique_ptr<ASTNode> &&root, Environment startingEnv = {}) : 
      root{std::move(root)}, startingEnv{std::move(startingEnv)} {}
  const ASTNode &getParent() const { return *root; }
  void setRoot(std::unique_ptr<ASTNode> &&root) { root.swap(this->root); }
  Environment getEnv() const {
      return startingEnv;
  }
  coro::Task<> accept(ASTVisitor &visitor) {
    auto coroutine = root->accept(visitor);
    while (not coroutine.isDone()) {
      co_await coroutine;
    }
  }

private:
  std::unique_ptr<ASTNode> root;
  Environment startingEnv;
};

} // namespace AST

#endif
