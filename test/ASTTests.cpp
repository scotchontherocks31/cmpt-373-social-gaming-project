#include "ASTNode.h"
#include "ASTVisitor.h"
#include "ExpressionParser.h"
#include "gtest/gtest.h"

#include <iostream>
#include <map>
#include <string>
#include <task.h>
#include <variant>

using namespace testing;

TEST(ExpressionNodes, RecursiveNesting) {

  std::unique_ptr<AST::BinaryNode> bin = std::make_unique<AST::BinaryNode>(
      std::make_unique<AST::VariableExpression>(std::string{"player"}),
      std::make_unique<AST::VariableExpression>(std::string{"size"}),
      Type::DOT);

  std::unique_ptr<AST::UnaryNode> un = std::make_unique<AST::UnaryNode>(
      std::make_unique<AST::VariableExpression>(std::string{"random"}),
      Type::DOT);

  std::unique_ptr<AST::BinaryNode> bin3 = std::make_unique<AST::BinaryNode>(
      std::move(bin), std::move(un), Type::DOT);
}

TEST(ExpressionNodes, ExpressionFunctions) {

  std::vector<std::unique_ptr<AST::ExpressionNode>> args;

  std::unique_ptr<AST::ExpressionNode> arg1 =
      std::make_unique<AST::VariableExpression>(std::string{"player"});
  std::unique_ptr<AST::BinaryNode> arg2 = std::make_unique<AST::BinaryNode>(
      std::make_unique<AST::VariableExpression>(std::string{"weapon"}),
      std::make_unique<AST::VariableExpression>(std::string{"beats"}),
      Type::EQUALS);

  args.push_back(std::move(arg1));
  args.push_back(std::move(arg2));

  std::unique_ptr<AST::FunctionCallNode> func =
      std::make_unique<AST::FunctionCallNode>(
          std::make_unique<AST::VariableExpression>(std::string{"collect"}),
          std::move(args));

  EXPECT_EQ(func->getFunctionName().getLexeme(), "collect");
}
