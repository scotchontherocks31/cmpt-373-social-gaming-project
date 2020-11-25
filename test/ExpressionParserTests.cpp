#include "ExpressionParser.h"
#include "RDP.h"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>
using namespace testing;

TEST(ExpressionParser, simple) {

  AST::RDP rdp("Weapon==Player");

  std::unique_ptr<AST::ExpressionNode> result = rdp.parse_S();

  std::unique_ptr<AST::BinaryNode> expected = std::make_unique<AST::BinaryNode>(
      std::make_unique<AST::VariableExpression>(std::string{"Weapon"}),
      std::make_unique<AST::VariableExpression>(std::string{"Player"}),
      Type::EQUALS);
}