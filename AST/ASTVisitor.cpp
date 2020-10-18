#include "ASTVisitor.h"

namespace AST {
coro::Task<> ASTVisitor::visit(GlobalMessage &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(FormatNode &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Variable &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(VarDeclaration &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Rules &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(ParallelFor &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visitHelper(GlobalMessage &) { co_return; }
coro::Task<> ASTVisitor::visitHelper(FormatNode &) { co_return; }
coro::Task<> ASTVisitor::visitHelper(ParallelFor &) { co_return; }
coro::Task<> ASTVisitor::visitHelper(Rules &) { co_return; }
coro::Task<> ASTVisitor::visitHelper(Variable &) { co_return; }
coro::Task<> ASTVisitor::visitHelper(VarDeclaration &) { co_return; }

} // namespace AST
