#include "CFGParser.h"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>
using namespace testing;

TEST(parseToCFGExpressionTest, simpleDot) {
  std::string input = "winners.size";
  std::vector<CFGExpression> result = parseToCFGExpression(input);

  std::vector<CFGExpression> expected = {{CFGExpression(Type::ID, "winners")},
                                         {CFGExpression(Type::DOT, ".")},
                                         {CFGExpression(Type::ID, "size")}};
  ASSERT_EQ(result.size(), expected.size());

  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i].getType(), expected[i].getType());
    EXPECT_EQ(result[i].getValue(), expected[i].getValue());
  }
}

TEST(parseToCFGExpressionTest, simpleEquals) {
  std::string input = "winners==size";
  std::vector<CFGExpression> result = parseToCFGExpression(input);

  std::vector<CFGExpression> expected = {{CFGExpression(Type::ID, "winners")},
                                         {CFGExpression(Type::EQUALS, "==")},
                                         {CFGExpression(Type::ID, "size")}};
  ASSERT_EQ(result.size(), expected.size());

  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i].getType(), expected[i].getType());
    EXPECT_EQ(result[i].getValue(), expected[i].getValue());
  }
}

TEST(parseToCFGExpressionTest, complex) {
  std::string input = "! words == f(x,y)";
  std::vector<CFGExpression> result = parseToCFGExpression(input);

  std::vector<CFGExpression> expected = {
      {CFGExpression(Type::NOT, "!")},     {CFGExpression(Type::ID, "words")},
      {CFGExpression(Type::EQUALS, "==")}, {CFGExpression(Type::ID, "f")},
      {CFGExpression(Type::OPENPAR, "(")}, {CFGExpression(Type::ID, "x")},
      {CFGExpression(Type::COMMA, ",")},   {CFGExpression(Type::ID, "y")},
      {CFGExpression(Type::CLOSEPAR, ")")}};
  ASSERT_EQ(result.size(), expected.size());

  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i].getType(), expected[i].getType());
    EXPECT_EQ(result[i].getValue(), expected[i].getValue());
  }
}

TEST(parseToCFGExpressionTest, complex2) {
  std::string input =
      "players.elements.collect(player, player.weapon <= weapon.beats)";
  std::vector<CFGExpression> result = parseToCFGExpression(input);

  std::vector<CFGExpression> expected = {
      {CFGExpression(Type::ID, "players")},
      {CFGExpression(Type::DOT, ".")},
      {CFGExpression(Type::ID, "elements")},
      {CFGExpression(Type::DOT, ".")},
      {CFGExpression(Type::ID, "collect")},
      {CFGExpression(Type::OPENPAR, "(")},
      {CFGExpression(Type::ID, "player")},
      {CFGExpression(Type::COMMA, ",")},
      {CFGExpression(Type::ID, "player")},
      {CFGExpression(Type::DOT, ".")},
      {CFGExpression(Type::ID, "weapon")},
      {CFGExpression(Type::LESSEQUALS, "<=")},
      {CFGExpression(Type::ID, "weapon")},
      {CFGExpression(Type::DOT, ".")},
      {CFGExpression(Type::ID, "beats")},
      {CFGExpression(Type::CLOSEPAR, ")")}};

  ASSERT_EQ(result.size(), expected.size());

  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i].getType(), expected[i].getType());
    EXPECT_EQ(result[i].getValue(), expected[i].getValue());
  }
}