#include "ASTNode.h"
#include "ASTVisitor.h"
#include "gtest/gtest.h"

#include <iostream>
#include <map>
#include <string>
#include <task.h>
#include <variant>

// using namespace AST;
using namespace testing;

class MockCommunicator : public AST::Communicator {
public:
  MOCK_METHOD(void, sendGlobalMessage, (std::string message), (override));
};

TEST(ASTprinter, GlobalMessageWithoutExpression) {

  auto enviro = AST::Environment{nullptr};
  MockCommunicator comm{};
  AST::Interpreter interp = AST::Interpreter{std::move(enviro), comm};

  std::unique_ptr<AST::GlobalMessage> mess =
      std::make_unique<AST::GlobalMessage>(
          std::make_unique<AST::FormatNode>(std::string{"Message One"}));

  auto root = AST::AST(std::move(mess));

  // capture print
  testing::internal::CaptureStdout();

  AST::Printer printer = AST::Printer{std::cout};
  auto task = root.accept(printer);
  while (task.resume()) {
  }

  // retrieve print
  std::string output = testing::internal::GetCapturedStdout();

  std::string answer = "(GlobalMessage (FormatNode \"Message One\" ))";
  EXPECT_EQ(output, answer);
}

TEST(ASTprinter, ParallelForandInput) {

  auto enviro = AST::Environment{nullptr};
  MockCommunicator comm{};
  AST::Interpreter interp = AST::Interpreter{std::move(enviro), comm};

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
  testing::internal::CaptureStdout();

  AST::Printer printer = AST::Printer{std::cout};
  auto task = root.accept(printer);
  while (task.resume()) {
  }

  // retrieve print
  std::string output = testing::internal::GetCapturedStdout();

  std::string answer =
      "(ParallelFor (Variable \"players\" )(VarDeclaration \"player\" )(Rules "
      "(GlobalMessage (FormatNode \"Message One\" ))(InputText (FormatNode "
      "\"How are you\" )(Variable \"player\" )(VarDeclaration \"response\" "
      "))))";
  EXPECT_EQ(output, answer);
}
