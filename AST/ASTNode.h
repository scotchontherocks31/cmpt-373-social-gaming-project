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

enum Mode {EXACT, AT_MOST, TRACK};

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

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
  std::string format;
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

class Condition : public ASTNode {
public:
  explicit Condition(std::string cond) : cond{std::move(cond)} {}
  const std::string &getCond() const { return cond; }
private:
  std::string cond;
};

class Key : public ASTNode {
public:
  explicit Key(std::string attribute) : attribute{std::move(attribute)} {}
  const std::string &getAttr() const { return attribute; }
private:
  std::string attribute;
};

class IntContainer : public ASTNode {
public:
  explicit IntContainer(int value) : value{value} {}
  const int &getValue() const { return value; }
private:
  int value;
};

class Message : public ASTNode {
public:
  explicit Message(std::unique_ptr<VarDeclaration> &&to,
                   std::unique_ptr<FormatNode> &&value){
    appendChild(std::move(to));
    appendChild(std::move(value));
  }
  
private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
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

class Scores : public ASTNode {
public:
  explicit Scores(std::unique_ptr<Variable> &&score,
                  std::unique_ptr<Condition> &&ascending){
    appendChild(std::move(score));
    appendChild(std::move(ascending));
  }
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

class AllSwitchCases : public ASTNode {
public:
  explicit AllSwitchCases() = default;
  void appendChild(std::unique_ptr<ASTNode> &&child) {
    ASTNode::appendChild(std::move(child));
  }
};

class AllWhenCases : public ASTNode {
public:
  explicit AllWhenCases() = default;
  void appendChild(std::unique_ptr<ASTNode> &&child) {
    ASTNode::appendChild(std::move(child));
  }
};



class WhenCase : public ASTNode {
public:
  WhenCase(std::unique_ptr<Condition> &&cond,
                std::unique_ptr<Rules> &&rules){
    appendChild(std::move(cond));
    appendChild(std::move(rules));
  }
};

class SwitchCase : public ASTNode {
public:
  SwitchCase(std::unique_ptr<FormatNode> &&value,
                      std::unique_ptr<Rules> &&rules){
    appendChild(std::move(value));
    appendChild(std::move(rules));
  }
};

class InputChoice : public ASTNode {
public:
  explicit InputChoice(std::unique_ptr<FormatNode> &&prompt,
                       std::unique_ptr<Variable> &&to,
                       std::unique_ptr<Variable> &&choices,
                       std::unique_ptr<VarDeclaration> &&result) {
    appendChild(std::move(prompt));
    appendChild(std::move(to));
    appendChild(std::move(choices));
    appendChild(std::move(result));
  }

  void addTimeout(std::unique_ptr<IntContainer> &&duration) {//Optional timeout case
    appendChild(std::move(duration));
  }
  const FormatNode &getPrompt() const {
    return *static_cast<FormatNode *>(children[0].get());
  }
  const Variable &getTo() const {
    return *static_cast<Variable *>(children[1].get());
  }
  const Variable &getChoices() const {
    return *static_cast<Variable *>(children[3].get());
  }
  const VarDeclaration &getResult() const {
    return *static_cast<VarDeclaration *>(children[4].get());
  }
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

  void addTimeout(std::unique_ptr<IntContainer> &&duration) {//Optional timeout case
    appendChild(std::move(duration));
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

class InputVote : public ASTNode {
public:
  explicit InputVote(std::unique_ptr<FormatNode> &&prompt,
                     std::unique_ptr<Variable> &&to,
                     std::unique_ptr<Variable> &&choices,
                     std::unique_ptr<VarDeclaration> &&result) {
    appendChild(std::move(prompt));
    appendChild(std::move(to));
    appendChild(std::move(choices));
    appendChild(std::move(result));
  }

  void addTimeout(std::unique_ptr<IntContainer> &&duration) {//Optional timeout case
    appendChild(std::move(duration));
  }
  const FormatNode &getPrompt() const {
    return *static_cast<FormatNode *>(children[0].get());
  }
  const Variable &getTo() const {
    return *static_cast<Variable *>(children[1].get());
  }
  const Variable &getChoices() const {
    return *static_cast<Variable *>(children[3].get());
  }
  const VarDeclaration &getResult() const {
    return *static_cast<VarDeclaration *>(children[4].get());
  }
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

class ForEach : public ASTNode {
public:
  ForEach(std::unique_ptr<Variable> &&variable,
          std::unique_ptr<VarDeclaration> &&varDeclaration,
          std::unique_ptr<Rules> &&rules) {
    appendChild(std::move(variable));
    appendChild(std::move(varDeclaration));
    appendChild(std::move(rules));
  }
};

class Loop : public ASTNode {
public:
  Loop(std::unique_ptr<Condition> &&condition,
       std::unique_ptr<Rules> &&rules){
    appendChild(std::move(condition));
    appendChild(std::move(rules));
  }
};

class InParallel : public ASTNode {
public:
  InParallel(std::unique_ptr<Rules> &&rules){
    appendChild(std::move(rules));
  }
};

class Reverse : public ASTNode {
public:
  Reverse(std::unique_ptr<Variable> &&variable){
    appendChild(std::move(variable));
  }
};

class Switch : public ASTNode {
public:
  Switch(std::unique_ptr<FormatNode> &&value,
         std::unique_ptr<Variable> &&list,
         std::unique_ptr<AllSwitchCases> &&cases){
    appendChild(std::move(value));
    appendChild(std::move(list));
    appendChild(std::move(cases));
  }
};

class When : public ASTNode {
public:
  When(std::unique_ptr<AllWhenCases> &&cases){
    appendChild(std::move(cases));
  }
};

class Extend : public ASTNode {
public:
  Extend(std::unique_ptr<Variable> &&target,
         std::unique_ptr<Variable> &&list){
    appendChild(std::move(target));
    appendChild(std::move(list));
  }
};

class Shuffle : public ASTNode {
public:
  Shuffle(std::unique_ptr<Variable> &&variable){
    appendChild(std::move(variable));
  }
};

class Sort : public ASTNode {
public:
  Sort(std::unique_ptr<Variable> &&listToSort){
    appendChild(std::move(listToSort));
  }

  void addAttribute(std::unique_ptr<Key> &&attr){
    appendChild(std::move(attr));
  }
};

class Deal : public ASTNode{
public:
  Deal(std::unique_ptr<Variable> &&fromList,
       std::unique_ptr<Variable> &&toList,
       std::unique_ptr<IntContainer> &&count){
    appendChild(std::move(fromList));
    appendChild(std::move(toList));
    appendChild(std::move(count));
  }
};

class Discard : public ASTNode{
public:
  Discard(std::unique_ptr<Variable> &&fromList,
          std::unique_ptr<IntContainer> &&count){
    appendChild(std::move(fromList));
    appendChild(std::move(count));
  }
};

class Add : public ASTNode{
public:
  Add(std::unique_ptr<Variable> &&intVar,
      std::unique_ptr<IntContainer> &&Add){
    appendChild(std::move(intVar));
    appendChild(std::move(Add));
  }
};

class Timer : public ASTNode{//needs to be modified for modes
public:
  Timer(std::unique_ptr<IntContainer> &&duration,
        std::unique_ptr<Variable> &&mode,
        std::unique_ptr<Rules> &&rules){
    appendChild(std::move(duration));
    appendChild(std::move(mode));
    appendChild(std::move(rules));
  }

  void addFlag(std::unique_ptr<Condition> &&cond){
    appendChild(std::move(cond));
  }
};

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
