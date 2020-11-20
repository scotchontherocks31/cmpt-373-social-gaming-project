#include "ASTNode.h"
#include "ASTVisitor.h"
#include "gtest/gtest.h"
#include "ExpressionParser.h"

#include <iostream>
#include <map>
#include <string>
#include <task.h>
#include <variant>

using namespace testing;



TEST(ExpressionNodes, RecursiveNesting ) {




  std::unique_ptr<AST::BinaryNode> bin = std::make_unique<AST::BinaryNode>(
      std::make_unique<AST::Variable>(std::string{"player"}),
      std::make_unique<AST::Variable>(std::string{"size"}),
      Type::DOT);

  std::unique_ptr<AST::UnaryNode> un = std::make_unique<AST::UnaryNode>(
  std::make_unique<AST::Variable>(std::string{"size"}),
  Type::DOT);

  std::unique_ptr<AST::BinaryNode> bin3 = std::make_unique<AST::BinaryNode>(
      std::move(bin),
      std::move(un),
      Type::DOT);

  

  std::unique_ptr<AST::GlobalMessage> mess =
      std::make_unique<AST::GlobalMessage>(
          std::make_unique<AST::FormatNode>(std::string{"Message One"}));

// enum class Type { // name it better
//   OPENPAR,
//   CLOSEPAR,
//   ID,
//   DOT,
}
