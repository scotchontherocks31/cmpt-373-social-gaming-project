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

  auto enviro = std::make_unique<AST::Environment>();
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

  auto enviro = std::make_unique<AST::Environment>();
  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(enviro), printComm};

  std::unique_ptr<AST::InputText> in = std::make_unique<AST::InputText>(
      std::make_unique<AST::FormatNode>(std::string{"How are you"}),
      std::make_unique<AST::Variable>(std::string{"player"}),
      std::make_unique<AST::VarDeclaration>(std::string{"response"}));

  std::unique_ptr<AST::GlobalMessage> mess =
      std::make_unique<AST::GlobalMessage>(
          std::make_unique<AST::FormatNode>(std::string{"Message One"}));

  std::unique_ptr<AST::Rules> rule = std::make_unique<AST::Rules>();
  rule->appendChild(std::move(mess));
  rule->appendChild(std::move(in));

  std::unique_ptr<AST::ParallelFor> par = std::make_unique<AST::ParallelFor>(
      std::make_unique<AST::Variable>(std::string{"players"}),
      std::make_unique<AST::VarDeclaration>(std::string{"player"}),
      std::move(rule));

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
      "(ParallelFor(Variable\"players\")(VarDeclaration\"player\")(Rules("
      "GlobalMessage(FormatNode\"Message One\"))(InputText(FormatNode\"How "
      "are you\")(Variable\"player\")(VarDeclaration\"response\"))))";
  EXPECT_EQ(output, answer);
}

TEST(ExpressionNodes, FormatNodeExpressionParsing) {
  auto parent = std::make_unique<AST::Environment>();

  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(parent), printComm};

  auto parser = AST::JSONToASTParser(std::string{
      "{\"configuration\":{\"name\":\"Rock,Paper,Scissors\",\"playercount\":{"
      "\"min\":2,\"max\":4},\"audience\":false,\"setup\":{\"Rounds\":10}},"
      "\"constants\":{},\"variables\":{},\"per-player\":{},\"per-audience\":{},"
      "\"rules\":[{\"rule\":\"global-message\",\"value\":\"{player.name}is "
      "your favorite person,fav food is {player.food},and # of players "
      "is{players.size}\"}]}"});

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

TEST(ExpressionNodes, BinaryNodeVisitorDot) {
  auto parent = std::make_unique<AST::Environment>();
  AST::Json playerJson;
  playerJson["id"] = 1;
  playerJson["name"] = "Mike Tyson";
  playerJson["food"] = "Jones";

  AST::Symbol symbol = AST::Symbol{AST::DSLValue{playerJson}, false};

  AST::Environment::Name key = "player";
  parent->allocate(key, symbol);
  EXPECT_EQ(symbol.dsl, parent->find(key));

  AST::ExpressionASTParser rdp("player.name");
  std::unique_ptr<AST::ExpressionNode> ast = rdp.parse_S();

  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(parent), printComm};
  auto root = AST::AST(std::move(ast));
  auto task = root.accept(interp);
  while (task.resume()) {
  }

  auto env = interp.getEnvironment();

  DSLValue output = *(env->getReturnValue());
  DSLValue answer{"Mike Tyson"};

  EXPECT_EQ(output, answer);
}

TEST(ExpressionNodes, BinaryNodeVisitorDotList) {

  auto parent = std::make_unique<AST::Environment>();
  Json myJson = Json::parse(
      "[{\"name\":\"Rock\",\"weapon\":\"Scissors\"},{\"name\":\"Paper\","
      "\"weapon\":\"Rock\"},{\"name\":\"Scissors\",\"weapon\":\"Paper\"}]");

  AST::Symbol symbol = AST::Symbol{AST::DSLValue{myJson}, false};

  AST::Environment::Name key = "players";
  parent->allocate(key, symbol);
  EXPECT_EQ(symbol.dsl, parent->find(key));

  AST::ExpressionASTParser rdp("players.weapon");
  std::unique_ptr<AST::ExpressionNode> ast = rdp.parse_S();

  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(parent), printComm};
  auto root = AST::AST(std::move(ast));
  auto task = root.accept(interp);
  while (task.resume()) {
  }

  auto env = interp.getEnvironment();
  DSLValue output = *(env->getReturnValue());

  std::vector<DSLValue> answer;

  answer.push_back(DSLValue{"Scissors"});
  answer.push_back(DSLValue{"Rock"});
  answer.push_back(DSLValue{"Paper"});

  for (size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(*output[i], answer[i]);
  }
}

TEST(ExpressionNodes, BinaryNodeVisitorDotListElements) {

  auto parent = std::make_unique<AST::Environment>();
  Json myJson = Json::parse(
      "[{\"name\":\"Rock\",\"weapon\":\"Scissors\"},{\"name\":\"Paper\","
      "\"weapon\":\"Rock\"},{\"name\":\"Scissors\",\"weapon\":\"Paper\"}]");

  AST::Symbol symbol = AST::Symbol{AST::DSLValue{myJson}, false};

  AST::Environment::Name key = "players";
  parent->allocate(key, symbol);
  EXPECT_EQ(symbol.dsl, parent->find(key));

  AST::ExpressionASTParser rdp("players.elements.weapon");
  std::unique_ptr<AST::ExpressionNode> ast = rdp.parse_S();

  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(parent), printComm};
  auto root = AST::AST(std::move(ast));
  auto task = root.accept(interp);
  while (task.resume()) {
  }

  auto env = interp.getEnvironment();
  DSLValue output = *(env->getReturnValue());
  std::vector<DSLValue> answer;

  answer.push_back(DSLValue{"Scissors"});
  answer.push_back(DSLValue{"Rock"});
  answer.push_back(DSLValue{"Paper"});

  for (size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(*output[i], answer[i]);
  }
}

TEST(ExpressionNodes, BinaryNodeVisitorEquals) {

  // Test Binary Node Equals
  auto parent = std::make_unique<AST::Environment>();

  AST::Symbol symbol = AST::Symbol{AST::DSLValue{100}, false};
  AST::Environment::Name key = "player1Score";
  parent->allocate(key, symbol);

  AST::Symbol symbol2 = AST::Symbol{AST::DSLValue{100}, false};
  AST::Environment::Name key2 = "player2Score";
  parent->allocate(key2, symbol2);

  AST::ExpressionASTParser rdp("player1Score == player2Score");
  std::unique_ptr<AST::ExpressionNode> ast = rdp.parse_S();

  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(parent), printComm};
  auto root = AST::AST(std::move(ast));
  auto task = root.accept(interp);
  while (task.resume()) {
  }

  auto env = interp.getEnvironment();
  DSLValue answer{true};
  EXPECT_EQ(*(env->getReturnValue()), answer);

  auto parent2 = std::make_unique<AST::Environment>();

  parent2->allocate(key, symbol);

  AST::Symbol symbol3 = AST::Symbol{AST::DSLValue{200}, false};
  AST::Environment::Name key3 = "player3Score";
  parent2->allocate(key3, symbol3);

  AST::ExpressionASTParser rdp2("player1Score == player3Score");
  std::unique_ptr<AST::ExpressionNode> ast2 = rdp2.parse_S();
  AST::PrintCommunicator printComm2{};
  AST::Interpreter interp2 = AST::Interpreter{std::move(parent2), printComm2};
  auto root2 = AST::AST(std::move(ast2));
  auto task2 = root2.accept(interp2);
  while (task2.resume()) {
  }

  auto env2 = interp2.getEnvironment();
  DSLValue answer2{false};
  EXPECT_EQ(*(env2->getReturnValue()), answer2);
}

TEST(ExpressionNodes, BinaryNodeVisitorNotEquals) {

  auto parent = std::make_unique<AST::Environment>();

  AST::Symbol symbol = AST::Symbol{AST::DSLValue{100}, false};
  AST::Environment::Name key = "player1Score";
  parent->allocate(key, symbol);

  AST::Symbol symbol2 = AST::Symbol{AST::DSLValue{100}, false};
  AST::Environment::Name key2 = "player2Score";
  parent->allocate(key2, symbol2);

  AST::ExpressionASTParser rdp("player1Score != player2Score");
  std::unique_ptr<AST::ExpressionNode> ast = rdp.parse_S();
  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(parent), printComm};
  auto root = AST::AST(std::move(ast));
  auto task = root.accept(interp);
  while (task.resume()) {
  }

  auto env = interp.getEnvironment();
  DSLValue answer{false};
  EXPECT_EQ(*(env->getReturnValue()), answer);
}

TEST(ExpressionNodes, BinaryNodeVisitorGreaterThan) {

  auto parent = std::make_unique<AST::Environment>();

  AST::Symbol symbol = AST::Symbol{AST::DSLValue{100}, false};
  AST::Environment::Name key = "player1Score";
  parent->allocate(key, symbol);

  AST::Symbol symbol2 = AST::Symbol{AST::DSLValue{200}, false};
  AST::Environment::Name key2 = "player2Score";
  parent->allocate(key2, symbol2);

  AST::ExpressionASTParser rdp("player1Score > player2Score");
  std::unique_ptr<AST::ExpressionNode> ast = rdp.parse_S();
  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(parent), printComm};
  auto root = AST::AST(std::move(ast));
  auto task = root.accept(interp);
  while (task.resume()) {
  }

  auto env = interp.getEnvironment();
  DSLValue answer{false};
  EXPECT_EQ(*(env->getReturnValue()), answer);
}

TEST(ExpressionNodes, BinaryNodeVisitorLessThan) {

  auto parent = std::make_unique<AST::Environment>();

  AST::Symbol symbol = AST::Symbol{AST::DSLValue{100}, false};
  AST::Environment::Name key = "player1Score";
  parent->allocate(key, symbol);

  AST::Symbol symbol2 = AST::Symbol{AST::DSLValue{50}, false};
  AST::Environment::Name key2 = "player2Score";
  parent->allocate(key2, symbol2);

  AST::ExpressionASTParser rdp("player1Score < player2Score");
  std::unique_ptr<AST::ExpressionNode> ast = rdp.parse_S();
  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(parent), printComm};
  auto root = AST::AST(std::move(ast));
  auto task = root.accept(interp);
  while (task.resume()) {
  }

  auto env = interp.getEnvironment();
  DSLValue answer{false};
  EXPECT_EQ(*(env->getReturnValue()), answer);
}

TEST(ExpressionNodes, BinaryNodeVisitorLessThanEquals) {

  auto parent = std::make_unique<AST::Environment>();

  AST::Symbol symbol = AST::Symbol{AST::DSLValue{101}, false};
  AST::Environment::Name key = "player1Score";
  parent->allocate(key, symbol);

  AST::Symbol symbol2 = AST::Symbol{AST::DSLValue{100}, false};
  AST::Environment::Name key2 = "player2Score";
  parent->allocate(key2, symbol2);

  AST::ExpressionASTParser rdp("player1Score <= player2Score");
  std::unique_ptr<AST::ExpressionNode> ast = rdp.parse_S();
  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(parent), printComm};
  auto root = AST::AST(std::move(ast));
  auto task = root.accept(interp);
  while (task.resume()) {
  }

  auto env = interp.getEnvironment();
  DSLValue answer{false};
  EXPECT_EQ(*(env->getReturnValue()), answer);
}

TEST(ExpressionNodes, UnaryNot) {

  // Test Binary Node Equals
  auto parent = std::make_unique<AST::Environment>();

  AST::Symbol symbol = AST::Symbol{AST::DSLValue{100}, false};
  AST::Environment::Name key = "player1Score";
  parent->allocate(key, symbol);

  AST::Symbol symbol2 = AST::Symbol{AST::DSLValue{200}, false};
  AST::Environment::Name key2 = "player2Score";
  parent->allocate(key2, symbol2);

  AST::ExpressionASTParser rdp("!(player1Score == player2Score)");
  std::unique_ptr<AST::ExpressionNode> ast = rdp.parse_S();

  AST::PrintCommunicator printComm{};
  AST::Interpreter interp = AST::Interpreter{std::move(parent), printComm};
  auto root = AST::AST(std::move(ast));
  auto task = root.accept(interp);
  while (task.resume()) {
  }

    auto env = interp.getEnvironment();
    DSLValue answer{true};
    EXPECT_EQ(*(env->getReturnValue()), answer);
}