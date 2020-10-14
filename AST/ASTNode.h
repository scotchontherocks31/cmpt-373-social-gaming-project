#ifndef AST_NODE_H
#define AST_NODE_H

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace AST {

class ASTVisitor;

class ASTNode {
public:
  int getChildrenCount() const { return this->numChildren; }
  const std::vector<ASTNode const *> getChildren() const {
    std::vector<ASTNode const *> returnValue;
    for (auto &x : children) {
      returnValue.push_back(x.get());
    }
    return returnValue;
  }
  const ASTNode &getParent() const { return *parent; }
  void setParent(ASTNode *parent) { parent = parent; }
  void accept(ASTVisitor &visitor) { acceptHelper(visitor); }
  void acceptForChildren(ASTVisitor &visitor) {
    acceptForChildrenHelper(visitor);
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
  virtual void acceptHelper(ASTVisitor &visitor) = 0;
  virtual void acceptForChildrenHelper(ASTVisitor &visitor) = 0;
};

class FormatNode : public ASTNode {
public:
  FormatNode(std::string format) : format{std::move(format)} {}
  const std::string &getFormat() const { return format; }

private:
  virtual void acceptHelper(ASTVisitor &visitor) override;
  virtual void acceptForChildrenHelper(ASTVisitor &visitor) override;
  std::string format;
};

class GlobalMessage : public ASTNode {
public:
  GlobalMessage(std::unique_ptr<FormatNode> &&formatNode) {
    appendChild(std::move(formatNode));
  }
  const FormatNode &getFormatNode() const {
    return *static_cast<FormatNode *>(children[0].get());
  }

private:
  virtual void acceptHelper(ASTVisitor &visitor) override;
  virtual void acceptForChildrenHelper(ASTVisitor &visitor) override;
};

class AST {
public:
  AST(std::unique_ptr<ASTNode> &&root) : root{std::move(root)} {}
  const ASTNode &getParent() const { return *root; }
  void setRoot(std::unique_ptr<ASTNode> &&root) { root.swap(this->root); }
  void accept(ASTVisitor &visitor) { root->accept(visitor); }

private:
  std::unique_ptr<ASTNode> root;
};

} // namespace AST

#endif
