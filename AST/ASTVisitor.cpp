#include "ASTVisitor.h"

std::string formatString(std::string format,
                         const std::vector<std::string> &args) {
  auto pos = format.find("{}");
  size_t argIndex = 0;
  while (pos != std::string::npos and argIndex < args.size()) {
    format = format.replace(pos, 2, args.at(argIndex++));
    pos = format.find("{}");
  }
  return format;
}

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

coro::Task<> ASTVisitor::visit(BinaryNode &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> ASTVisitor::visit(UnaryNode &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> ASTVisitor::visit(VariableExpression &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> ASTVisitor::visit(FunctionCallNode &node) {
  auto coroutine = visitHelper(node);
  while (not coroutine.isDone()) {
    co_await coroutine;
  }
}

coro::Task<> Interpreter::visitHelper(FormatNode &node) {
  auto env = environment->createChildEnvironment();

  std::vector<std::string> args;
  for (auto &child : node.getChildren()) {
    co_await child->accept(*this);
    auto result = environment->getConstReturnValue();
    // Expressions have to resolve
    if (!result) {
      errorThrown = true;
      co_await coro::coroutine::suspend_always();
    }
    const DSLValue &dsl = *result;
    std::ostringstream sstream;
    sstream << dsl;
    args.push_back(sstream.str());
  }
  auto formattedStr = formatString(node.getFormat(), args);
  env.allocateReturn(Symbol{DSLValue{formattedStr}});
}

} // namespace AST
