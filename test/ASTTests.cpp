#include "ASTNode.h"
#include "ASTVisitor.h"
#include "CFGParser.h"
#include "DSLValue.h"
#include "Environment.h"
#include "ExpressionASTParser.h"
#include "Parser.h"

#include "gtest/gtest.h"

#include "json.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <task.h>
#include <variant>

using namespace testing;
using namespace AST;

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

TEST(ASTprinter, GlobalMessageWithoutExpression) {

  auto enviro = AST::PopulatedEnvironment{std::make_unique<AST::Environment>()};
  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(enviro), printComm};

  std::unique_ptr<AST::GlobalMessage> mess =
      std::make_unique<AST::GlobalMessage>(
          std::make_unique<AST::FormatNode>(std::string{"Message One"}));

  auto root = AST::AST(std::move(mess));

  std::stringstream stream;

  AST::Printer printer = AST::Printer{stream};

  auto task = root.accept(printer);
  while (task.resume()) {
  }

  std::string answer = "(GlobalMessage(FormatNode\"Message One\"))";
  std::string output = printer.returnOutput();

  EXPECT_EQ(output, answer);
}

TEST(ASTprinter, ParallelForandInput) {

  auto enviro = AST::PopulatedEnvironment{std::make_unique<AST::Environment>()};
  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(enviro), printComm};

  std::unique_ptr<AST::InputText> in = std::make_unique<AST::InputText>(
      std::make_unique<AST::FormatNode>(std::string{"How are you"}),
      std::make_unique<AST::Variable>(std::string{"player"}),
      std::make_unique<AST::Variable>(std::string{"response"}));

  std::unique_ptr<AST::GlobalMessage> mess =
      std::make_unique<AST::GlobalMessage>(
          std::make_unique<AST::FormatNode>(std::string{"Message One"}));

  std::unique_ptr<AST::Rules> rule = std::make_unique<AST::Rules>();
  rule->appendChild(std::move(mess));
  rule->appendChild(std::move(in));

  std::unique_ptr<AST::ParallelFor> par = std::make_unique<AST::ParallelFor>(
      std::make_unique<AST::Variable>(std::string{"players"}),
      std::make_unique<AST::Variable>(std::string{"player"}), std::move(rule));

  auto root = AST::AST(std::move(par));

  // capture print
  std::stringstream stream;
  AST::Printer printer = AST::Printer{stream};
  auto task = root.accept(printer);
  while (task.resume()) {
  }

  // retrieve print
  std::string output = printer.returnOutput();
  std::string answer =
      "(ParallelFor(Variable\"players\")(Variable\"player\")(Rules("
      "GlobalMessage(FormatNode\"Message One\"))(InputText(FormatNode\"How "
      "are you\")(Variable\"player\")(Variable\"response\"))))";
  EXPECT_EQ(output, answer);
}

TEST(ExpressionNodes, FormatNodeExpressionParsing) {

  auto parser = AST::JSONToASTParser(
      R"([
          {
            "rule": "global-message",
            "value": "{player.name}is your favorite person,fav food is {player.food},and # of players is{players.size}"
          }
        ])"_json);

  AST::AST ast = parser.parse(); // AST With GlobalMessage
  auto root = AST::AST(std::move(ast));
  std::stringstream stream;
  AST::Printer printer = AST::Printer{stream};
  auto task = root.accept(printer);
  while (task.resume()) {
  }
  std::string answer =
      "(Rules(GlobalMessage(FormatNode\"{player.name}is your favorite "
      "person,fav food is {player.food},and # of players is{players.size}\")))";
  std::string output = printer.returnOutput();

  EXPECT_EQ(output, answer);
}
