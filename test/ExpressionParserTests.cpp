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

TEST(ExpressionASTStructure, UnaryWithSize) {

  AST::RDP rdp("winners.size");

  std::unique_ptr<AST::ExpressionNode> result = rdp.parse_S();
  auto enviro = std::make_unique<AST::Environment>();
  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(enviro), printComm};
  auto root = AST::AST(std::move(result));

  std::stringstream stream;
  AST::Printer printer = AST::Printer{stream};

  auto task = root.accept(printer);
  while (task.resume()) {
  }

  std::string answer = "(BinaryNode:\".\"(VariableExpression\"winners\")("
                       "VariableExpression\"size\"))";
  std::string output = printer.returnOutput();
  EXPECT_EQ(output, answer);
}

TEST(ExpressionASTStructure, BinaryWithEquals) {

  AST::RDP rdp("winners.size == players.size");
  std::unique_ptr<AST::ExpressionNode> result = rdp.parse_S();

  auto enviro = std::make_unique<AST::Environment>();
  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(enviro), printComm};
  auto root = AST::AST(std::move(result));

  std::stringstream stream;
  AST::Printer printer = AST::Printer{stream};

  auto task = root.accept(printer);
  while (task.resume()) {
  }

  std::string answer =
      "(BinaryNode:\"==\"(BinaryNode:\".\"(VariableExpression\"winners\")("
      "VariableExpression\"size\"))(BinaryNode:\".\"("
      "VariableExpression\"players\")(VariableExpression\"size\")))";
  std::string output = printer.returnOutput();

  EXPECT_EQ(output, answer);
}

TEST(ExpressionASTStructure, UnaryandFunction) {

  AST::RDP rdp("!players.elements.weapon.contains(weapon.name)");
  std::unique_ptr<AST::ExpressionNode> result = rdp.parse_S();
  auto enviro = std::make_unique<AST::Environment>();
  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(enviro), printComm};

  auto root = AST::AST(std::move(result));
  std::stringstream stream;
  AST::Printer printer = AST::Printer{stream};
  auto task = root.accept(printer);
  while (task.resume()) {
  }

  std::string answer =
      "(UnaryNode:\"!\"(BinaryNode:\".\"(BinaryNode:\".\"(BinaryNode:\".\"("
      "VariableExpression\"players\")(VariableExpression\"elements\"))("
      "VariableExpression\"weapon\"))(FunctionCallNode:\"contains\"("
      "VariableExpression\"contains\")(BinaryNode:\".\"("
      "VariableExpression\"weapon\")(VariableExpression\"name\")))))";
  std::string output = printer.returnOutput();

  EXPECT_EQ(output, answer);
}
