#include "ASTVisitor.h"

namespace AST {
coro::Task<> ASTVisitor::visit(Message &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(GlobalMessage &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Scores &node) {
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
coro::Task<> ASTVisitor::visit(Condition &node) {
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
coro::Task<> ASTVisitor::visit(AllSwitchCases &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(AllWhenCases &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(SwitchCase &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(WhenCase &node) {
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
coro::Task<> ASTVisitor::visit(ForEach &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Loop &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(InParallel &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Switch &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(When &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> ASTVisitor::visit(Reverse &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Extend &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Shuffle &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Sort &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Deal &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Discard &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> ASTVisitor::visit(Add &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Timer &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> ASTVisitor::visit(InputChoice &node) {
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
coro::Task<> ASTVisitor::visit(InputVote &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

} // namespace AST
