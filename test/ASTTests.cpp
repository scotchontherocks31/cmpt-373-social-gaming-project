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
      std::make_unique<AST::VariableExpression>(std::string{"player"}),
      std::make_unique<AST::VariableExpression>(std::string{"size"}),
      Type::DOT);

  std::unique_ptr<AST::UnaryNode> un = std::make_unique<AST::UnaryNode>(
  std::make_unique<AST::VariableExpression>(std::string{"random"}),
  Type::DOT);

  std::unique_ptr<AST::BinaryNode> bin3 = std::make_unique<AST::BinaryNode>(
      std::move(bin),
      std::move(un),
      Type::DOT);

  
  //const AST::UnaryNode unRet = bin3->getArgTwo();

}


TEST(ExpressionNodes, ExpressionFunctions ) {

  // functionNode ( variablenode("upfrom") , Variablenode(1) )
 std::unique_ptr<AST::FunctionCallNode> func = std::make_unique<AST::FunctionCallNode>(
   std::make_unique<AST::VariableExpression>(std::string{"upfrom"}),
   std::make_unique<AST::VariableExpression>(std::string{"1"}));

  EXPECT_EQ(func->getFunctionName().getLexeme(),"upfrom");


//    - players.elements.collect(player, player.weapon == weapon.beats)
// 	- (funcNameVar,variable, variable, operator)
  std::unique_ptr<AST::BinaryNode> binColl = std::make_unique<AST::BinaryNode>(
      std::make_unique<AST::VariableExpression>(std::string{"player.weapon "}),
      std::make_unique<AST::VariableExpression>(std::string{"weapon.beats"}),
      Type::EQUALS);

 std::unique_ptr<AST::FunctionCallNode> func2 = std::make_unique<AST::FunctionCallNode>(
   std::make_unique<AST::VariableExpression>(std::string{"collect"}),
   std::move(binColl));


    EXPECT_EQ(func2->getFunctionName().getLexeme(),"collect");

  

}


