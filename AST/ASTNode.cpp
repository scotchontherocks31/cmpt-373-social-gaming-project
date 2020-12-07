#include "ASTNode.h"
#include "ASTVisitor.h"
#include "Parser.h"

namespace AST {
coro::Task<> Message::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> GlobalMessage::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Scores::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> FormatNode::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Variable::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Rules::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> AllSwitchCases::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> AllWhenCases::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> SwitchCase::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> WhenCase::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
//RULES
coro::Task<> ParallelFor::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> ForEach::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Loop::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> InParallel::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Switch::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> When::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
//LIST
coro::Task<> Reverse::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Extend::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Shuffle::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Sort::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Deal::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Discard::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Add::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Timer::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
//INPUT NODES
coro::Task<> InputChoice::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> InputText::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> InputVote::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone())
    co_await coroutine;
  }


coro::Task<> BinaryNode::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> UnaryNode::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> VariableExpression::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> FunctionCallNode::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

} // namespace AST
