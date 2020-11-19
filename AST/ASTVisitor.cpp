#include "ASTVisitor.h"

namespace AST {
coro::Task<> ASTVisitor::visit(GlobalMessage &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(FormatNode &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Variable &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(VarDeclaration &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(InputText &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Rules &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(ParallelFor &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}


} // namespace AST
