#ifndef AST_NODE_H
#define AST_NODE_H

#include "CFGParser.h"
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
  int numChildren;
  void appendChild(std::unique_ptr<ASTNode> &&child) {
    children.push_back(std::move(child));
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) = 0;
};

class ExpressionNode : public ASTNode {};

class VariableExpression : public ExpressionNode
{
public:
  // TODO: Add types to check validity (eg. list vs bool)
  explicit VariableExpression(std::string lexeme) : lexeme{std::move(lexeme)} {}
  const std::string &getLexeme() const { return lexeme; }

private:
  std::string lexeme;
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class BinaryNode : public ExpressionNode
{
public:
  BinaryNode(std::unique_ptr<ExpressionNode> &&operandLeft,
             std::unique_ptr<ExpressionNode> &&operandRight, Type binaryOper)
  {
    appendChild(std::move(operandLeft));
    appendChild(std::move(operandRight));
    binaryOperator = binaryOper;
  }
  const ExpressionNode &getArgOne() const
  {
    return *static_cast<ExpressionNode *>(children[0].get());
  }
  const ExpressionNode &getArgTwo() const
  {
    return *static_cast<ExpressionNode *>(children[1].get());
  }
  const Type getBinaryOperator() { return binaryOperator; }

private:
  Type binaryOperator;
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class UnaryNode : public ExpressionNode
{
public:
  UnaryNode(std::unique_ptr<ExpressionNode> &&operand, Type unaryOper)
  {
    appendChild(std::move(operand));
    unaryOperator = unaryOper;
  }
  const ExpressionNode &getArgOne() const
  {
    return *static_cast<ExpressionNode *>(children[0].get());
  }
  const Type getUnaryOperator() { return unaryOperator; }

private:
  Type unaryOperator;
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class FunctionCallNode final : public ExpressionNode
{
public:
  FunctionCallNode(std::unique_ptr<ExpressionNode> &&functionName,
                   std::vector<std::unique_ptr<ExpressionNode>> args)
  {
    appendChild(std::move(functionName));
    for (auto &arg : args)
    {
      appendChild(std::move(arg));
    }
  }
  const VariableExpression &getFunctionName() const
  {
    return *static_cast<VariableExpression *>(children[0].get());
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class FormatNode : public ASTNode {
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

class Message : public ASTNode {
public:
  explicit Message(std::unique_ptr<Variable> &&to,      //assuming it has "player", just store variable name
                   std::unique_ptr<FormatNode> &&value) //No example to derive from, possibly no Expression needed
  {
    appendChild(std::move(to));
    appendChild(std::move(value));
  }
  Variable &getTo() const{
    return *static_cast<Variable *>(children[0].get());
  }
  FormatNode &getValue() const{
    return *static_cast<FormatNode *>(children[1].get());
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class GlobalMessage : public ASTNode { // parser complete
public:
  explicit GlobalMessage(std::unique_ptr<FormatNode> &&formatNode) {
    appendChild(std::move(formatNode));
  }
  FormatNode &getFormatNode() const {
    return *static_cast<FormatNode *>(children[0].get());
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Scores : public ASTNode {
public:
  explicit Scores(std::unique_ptr<Variable> &&score, //"wins" -> store variable name
                  const bool &ascending)             //Strong type the incoming boolean
  {
    appendChild(std::move(score));
    this->ascending = ascending;
  }

  Variable &getScore() const{
    return *static_cast<Variable *>(children[0].get());
  }
  bool getAscending() const{
    return this->ascending;
  }

private:
  bool ascending;
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

class AllSwitchCases : public ASTNode {//container for SwitchCase
public:
  explicit AllSwitchCases() = default;
  void appendChild(std::unique_ptr<ASTNode> &&child) {
    ASTNode::appendChild(std::move(child));
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class AllWhenCases : public ASTNode {//container for WhenCase
public:
  explicit AllWhenCases() = default;
  void appendChild(std::unique_ptr<ASTNode> &&child) {
    ASTNode::appendChild(std::move(child));
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class WhenCase : public ASTNode {
public:
  WhenCase(std::unique_ptr<ASTNode> &&cond, //"!players.elements.weapon.contains(weapon.name)" -> Expression
           std::unique_ptr<Rules> &&rules)
  {
    appendChild(std::move(cond));
    appendChild(std::move(rules));
  }
  ASTNode &getCond() const{
    return *static_cast<ASTNode *>(children[0].get());
  }
  Rules &getRules() const{
    return *static_cast<Rules *>(children[1].get());
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class SwitchCase : public ASTNode {
public:
  SwitchCase(std::unique_ptr<FormatNode> &&value, //Value to compare with original switch -> Expression
             std::unique_ptr<Rules> &&rules){
    appendChild(std::move(value));
    appendChild(std::move(rules));
  }
  FormatNode &getValue() const{
    return *static_cast<FormatNode *>(children[0].get());
  }
  Rules &getRules() const{
    return *static_cast<Rules *>(children[1].get());
  }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class InputChoice : public ASTNode {
public:
  explicit InputChoice(std::unique_ptr<FormatNode> &&prompt, //"{player.name}, choose your weapon!" -> Expression
                       std::unique_ptr<Variable> &&to,       //"player" -> just store variable name
                       std::unique_ptr<ASTNode> &&choices,   //"weapons.name" -> Expression
                       std::unique_ptr<ASTNode> &&result)    //"player.weapon" -> Expression
  {
    appendChild(std::move(prompt));
    appendChild(std::move(to));
    appendChild(std::move(choices));
    appendChild(std::move(result));
    this->timeout = 0;
  }

  void setTimeout(const int& timeout){
    this->timeout = timeout;
  }
  int getTimeout() const{
    return this->timeout;
  }
  const FormatNode &getPrompt() const {
    return *static_cast<FormatNode *>(children[0].get());
  }
  const Variable &getTo() const {
    return *static_cast<Variable *>(children[1].get());
  }
  const ASTNode &getChoices() const{
    return *static_cast<ASTNode *>(children[3].get());
  }
  const ASTNode &getResult() const {
    return *static_cast<ASTNode *>(children[4].get());
  }

private:
  int timeout;
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class InputText : public ASTNode {
public:
  explicit InputText(std::unique_ptr<FormatNode> &&prompt,     //"{player.name}, choose your weapon!" -> Expression
                     std::unique_ptr<Variable> &&to,           //"player" -> just store variable name
                     std::unique_ptr<ASTNode> &&result) //"player.weapon" -> Expression
  {
    appendChild(std::move(prompt));
    appendChild(std::move(to));
    appendChild(std::move(result));
    this->timeout = 0;
  }

  void setTimeout(const int& timeout){
    this->timeout = timeout;
  }
  int getTimeout() const{
    return this->timeout;
  }
  const FormatNode &getPrompt() const {
    return *static_cast<FormatNode *>(children[0].get());
  }
  const Variable &getTo() const {
    return *static_cast<Variable *>(children[1].get());
  }
  const ASTNode &getResult() const {
    return *static_cast<ASTNode *>(children[2].get());
  }

private:
  int timeout;
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class InputVote : public ASTNode {
public:
  explicit InputVote(std::unique_ptr<FormatNode> &&prompt, //"{player.name}, choose your weapon!" -> Expression
                     std::unique_ptr<ASTNode> &&to,        //List variable, will need Expression in case of "a.b"
                     std::unique_ptr<ASTNode> &&choices,   //"weapons.name" -> Expression
                     std::unique_ptr<ASTNode> &&result)    //"player.weapon" -> Expression
  {
    appendChild(std::move(prompt));
    appendChild(std::move(to));
    appendChild(std::move(choices));
    appendChild(std::move(result));
    this->timeout = 0;
  }

  void setTimeout(const int& timeout){
    this->timeout = timeout;
  }
  int getTimeout() const{
    return this->timeout;
  }
  const FormatNode &getPrompt() const {
    return *static_cast<FormatNode *>(children[0].get());
  }
  const ASTNode &getTo() const {
    return *static_cast<ASTNode *>(children[1].get());
  }
  const ASTNode &getChoices() const {
    return *static_cast<ASTNode *>(children[3].get());
  }
  const ASTNode &getResult() const
  {
    return *static_cast<ASTNode *>(children[4].get());
  }

private:
  int timeout;
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class ParallelFor : public ASTNode {
public:
  ParallelFor(std::unique_ptr<Variable> &&variable,       //"list": "players"
                                                          // won't require Expression
              std::unique_ptr<Variable> &&varDeclaration, //"element": "round", store as Variable string
              std::unique_ptr<Rules> &&rules)
  {
    appendChild(std::move(variable));
    appendChild(std::move(varDeclaration));
    appendChild(std::move(rules));
  }
  Variable &getListName() const {
    return *static_cast<Variable *>(children[0].get());
  }
  Variable &getElementName() const {
    return *static_cast<Variable *>(children[1].get());
  }
  Rules &getRules() const { return *static_cast<Rules *>(children[2].get()); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class ForEach : public ASTNode {
public:
  ForEach(std::unique_ptr<ASTNode> &&variable,        //"list:" "configuration.Rounds.upfrom(1)"
                                                      // Therefore, Expression
          std::unique_ptr<Variable> &&varDeclaration, //"element": "round", store as Variable string
          std::unique_ptr<Rules> &&rules)
  {
    appendChild(std::move(variable));
    appendChild(std::move(varDeclaration));
    appendChild(std::move(rules));
  }
  ASTNode &getList() const{
    return *static_cast<ASTNode *>(children[0].get());
  }
  Variable &getElement() const
  {
    return *static_cast<Variable *>(children[1].get());
  }
  Rules &getRules() const { return *static_cast<Rules *>(children[2].get()); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Loop : public ASTNode {
public:
  Loop(std::unique_ptr<ASTNode> &&condition, //<< Condition that may fail >> -> Expression
       std::unique_ptr<Rules> &&rules)
  {
    appendChild(std::move(condition));
    appendChild(std::move(rules));
  }
  ASTNode &getCond() const{
    return *static_cast<ASTNode *>(children[0].get());
  }
  Rules &getRules() const { return *static_cast<Rules *>(children[1].get()); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class InParallel : public ASTNode {
public:
  InParallel(std::unique_ptr<Rules> &&rules){
    appendChild(std::move(rules));
  }
  Rules &getRules() const { return *static_cast<Rules *>(children[0].get()); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Switch : public ASTNode {
public:
  Switch(std::unique_ptr<FormatNode> &&value, //<< value to switch upon >> -> Expression
         std::unique_ptr<Variable> &&list, //<< name of a constant list of allowable values >> no Expression needed
         std::unique_ptr<AllSwitchCases> &&cases)
  {
    appendChild(std::move(value));
    appendChild(std::move(list));
    appendChild(std::move(cases));
  }
  FormatNode &getValue() const{
    return *static_cast<FormatNode *>(children[0].get());
  }
  Variable &getList() const{
    return *static_cast<Variable *>(children[1].get());
  }
  AllSwitchCases &getAllSwitchCases() const { return *static_cast<AllSwitchCases *>(children[2].get()); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class When : public ASTNode {
public:
  When(std::unique_ptr<AllWhenCases> &&cases){
    appendChild(std::move(cases));
  }
  AllWhenCases &getAllWhenCases() const { return *static_cast<AllWhenCases *>(children[0].get()); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Reverse : public ASTNode
{
public:
  Reverse(std::unique_ptr<Variable> &&variable) //<< variable name of a list to reverse >>
  {
    appendChild(std::move(variable));
  }
  Variable &getList() const { return *static_cast<Variable *>(children[0].get()); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Extend : public ASTNode {
public:
  Extend(std::unique_ptr<Variable> &&target, //<< variable name of a list to extend with another list >>
         std::unique_ptr<ASTNode> &&list)    //"players.elements.collect(player, player.weapon == weapon.beats)"
                                             // Therefore, Expression for "list"
  {
    appendChild(std::move(target));
    appendChild(std::move(list));
  }
  Variable &getTarget() const { return *static_cast<Variable *>(children[0].get()); }
  ASTNode &getList() const { return *static_cast<ASTNode *>(children[1].get()); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Shuffle : public ASTNode {
public:
  Shuffle(std::unique_ptr<Variable> &&variable)       //<< variable name of a list to shuffle >>
  {
    appendChild(std::move(variable));
  }
  Variable &getList() const { return *static_cast<Variable *>(children[0].get()); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Sort : public ASTNode {
public:
  Sort(std::unique_ptr<Variable> &&listToSort)        //<< variable name of a list to sort >>
  {
    appendChild(std::move(listToSort));
  }

  Variable &getList() const { return *static_cast<Variable *>(children[0].get()); }
  void addAttribute(std::unique_ptr<ASTNode> &&attr) //<< Attribute of list elements to use for comparison.
                                                     //   Only valid when the list contains maps.>>
                                                     // Attribute has to be deciphered, therefore
                                                     // EXPRESSION
  {
    appendChild(std::move(attr));
    this->hasAttribute = true;
  }
  bool checkAttribute() const {return this->hasAttribute;}  //Run this before getAttribute, else Seg fault
  ASTNode &getAttribute() const { return *static_cast<ASTNode *>(children[1].get()); }

private:
  bool hasAttribute = false;
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Deal : public ASTNode{
public:
  Deal(std::unique_ptr<Variable> &&fromList, //<< variable name of a list to deal from >>
       std::unique_ptr<Variable> &&toList,   //<< variable name of a list or list attribute to deal to >>
       std::unique_ptr<ASTNode> &&count)     //Expression, as per example: "count": "winners.size"
  {
    appendChild(std::move(fromList));
    appendChild(std::move(toList));
    appendChild(std::move(count));
  }
  Variable &getFromList() const { return *static_cast<Variable *>(children[0].get()); }
  Variable &getToList() const { return *static_cast<Variable *>(children[1].get()); }
  ASTNode &getCount() const { return *static_cast<ASTNode *>(children[2].get()); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Discard : public ASTNode{
public:
  Discard(std::unique_ptr<ASTNode> &&fromList, //"winner.wins" -> Expression
          std::unique_ptr<ASTNode> &&count)    //Expression, as per example: "count": "winners.size"
  {
    appendChild(std::move(fromList));
    appendChild(std::move(count));
  }
  Variable &getToList() const { return *static_cast<Variable *>(children[0].get()); }
  ASTNode &getCount() const { return *static_cast<ASTNode *>(children[1].get()); }

private:
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Add : public ASTNode{
public:
  Add(std::unique_ptr<ASTNode> &&intVar, //"winner.wins" -> Expression
      const int &value)                  //<< integer literal or name of a variable or
                                         //  constant containing the value to add >>
                                         //  "value": 1 -> example is an integer literal...
  {
    appendChild(std::move(intVar));
    this->value = value;
  }
  ASTNode &getIntVar() const { return *static_cast<ASTNode *>(children[0].get()); }
  int getValue() { return this->value; }

private:
  int value;
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
};

class Timer : public ASTNode{//needs to be modified for modes
public:
  Timer(const int &duration,              //<< seconds >> -> has to be int
        std::unique_ptr<Variable> &&mode, //Store "exact" OR "at most" OR "track", possibly use enum to track?
        std::unique_ptr<Rules> &&rules)
  {
    this->duration = duration;
    appendChild(std::move(mode));
    appendChild(std::move(rules));
  }

  void addFlag(std::unique_ptr<ASTNode> &&cond) //<< variable that evaluates to false when a "track" timer
                                                //  has not expired and false afterward. >>
                                                // Guessing it's Expression
  {
    appendChild(std::move(cond));
    this->hasFlag = true;
  }
  int getDuration() { return this->duration; }
  Variable &getMode() const { return *static_cast<Variable *>(children[0].get()); }
  Rules &getCount() const { return *static_cast<Rules *>(children[1].get()); }
  bool checkFlag() const { return this->hasFlag; }
  ASTNode &getFlag() const { return *static_cast<ASTNode *>(children[2].get()); }

private:
  int duration;
  bool hasFlag = false;
  virtual coro::Task<> acceptHelper(ASTVisitor &visitor) override;
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
