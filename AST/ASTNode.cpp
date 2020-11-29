#include "ASTNode.h"
#include "ASTVisitor.h"
#include "Parser.h"

namespace AST {
coro::Task<> GlobalMessage::acceptHelper(ASTVisitor &visitor) {
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

coro::Task<> ParallelFor::acceptHelper(ASTVisitor &visitor) {
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

coro::Task<> VarDeclaration::acceptHelper(ASTVisitor &visitor) {
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

coro::Task<> InputText::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
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
