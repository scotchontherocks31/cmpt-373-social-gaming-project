#include "ASTNode.h"
#include "ASTVisitor.h"

namespace AST {
coro::Task<> GlobalMessage::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}

coro::Task<> FormatNode::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}

coro::Task<> ParallelFor::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}

coro::Task<> Variable::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}

coro::Task<> VarDeclaration::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}

coro::Task<> Rules::acceptHelper(ASTVisitor &visitor) {
  auto coroutine = visitor.visit(*this);
  while (not coroutine.isReady()) {
    co_await coroutine;
  }
}

} // namespace AST
