#include "ASTVisitor.h"

#if defined(CLANG)
namespace coroutine = std::experimental;
#elif defined(GCC)
namespace coroutine = std;
#endif

namespace AST {
coro::Task<> ASTVisitor::visit(Root &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}
coro::Task<> ASTVisitor::visit(Setup &node) {
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

coro::Task<> Interpreter::visitHelper(Setup &node) {
  auto setup = environment.getValue("setup");
  for (auto &key : setup.keys()) {
    auto value = setup.at(key);
    auto kind = value->get().at("kind");
    if (!kind) {
      continue;
    }
    auto prompt = value->get().at("prompt");
    communicator.sendToOwner(prompt->get().get<std::string>());
    auto messages = communicator.receiveFromOwner();
    while (messages.empty()) {
      co_await coroutine::suspend_always();
      messages = communicator.receiveFromOwner();
    }
    auto &kindStr = kind->get().get<std::string>().get();
  }
}

} // namespace AST
