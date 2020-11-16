#include "ExpressionParser.h"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>
using namespace testing;

TEST(ParseToTypeTest, simpleDot) {
  std::string input = "winners.size";
  std::vector<TokenType> result = parseToType(input);

  std::vector<TokenType> expected = {{TokenType(Type::ID, "winners")},
                                     {TokenType(Type::DOT, ".")},
                                     {TokenType(Type::ID, "size")}};
  ASSERT_EQ(result.size(), expected.size());

  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i].getType(), expected[i].getType());
    EXPECT_EQ(result[i].getValue(), expected[i].getValue());
  }
}

TEST(ParseToTypeTest, simpleEquals) {
  std::string input = "winners==size";
  std::vector<TokenType> result = parseToType(input);

  std::vector<TokenType> expected = {{TokenType(Type::ID, "winners")},
                                     {TokenType(Type::EQUALS, "==")},
                                     {TokenType(Type::ID, "size")}};
  ASSERT_EQ(result.size(), expected.size());

  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i].getType(), expected[i].getType());
    EXPECT_EQ(result[i].getValue(), expected[i].getValue());
  }
}

TEST(ParseToTypeTest, complex) {
  std::string input = "! words == f(x,y)";
  std::vector<TokenType> result = parseToType(input);

  std::vector<TokenType> expected = {
      {TokenType(Type::NOT, "!")},     {TokenType(Type::ID, "words")},
      {TokenType(Type::EQUALS, "==")}, {TokenType(Type::ID, "f")},
      {TokenType(Type::OPENPAR, "(")}, {TokenType(Type::ID, "x")},
      {TokenType(Type::COMMA, ",")},   {TokenType(Type::ID, "y")},
      {TokenType(Type::CLOSEPAR, ")")}};
  ASSERT_EQ(result.size(), expected.size());

  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i].getType(), expected[i].getType());
    EXPECT_EQ(result[i].getValue(), expected[i].getValue());
  }
}

TEST(ParseToTypeTest, complex2) {
  std::string input =
      "players.elements.collect(player, player.weapon <= weapon.beats)";
  std::vector<TokenType> result = parseToType(input);

  std::vector<TokenType> expected = {
      {TokenType(Type::ID, "players")},  {TokenType(Type::DOT, ".")},
      {TokenType(Type::ID, "elements")}, {TokenType(Type::DOT, ".")},
      {TokenType(Type::ID, "collect")},  {TokenType(Type::OPENPAR, "(")},
      {TokenType(Type::ID, "player")},   {TokenType(Type::COMMA, ",")},
      {TokenType(Type::ID, "player")},   {TokenType(Type::DOT, ".")},
      {TokenType(Type::ID, "weapon")},   {TokenType(Type::LESSEQUALS, "<=")},
      {TokenType(Type::ID, "weapon")},   {TokenType(Type::DOT, ".")},
      {TokenType(Type::ID, "beats")},    {TokenType(Type::CLOSEPAR, ")")}};

  ASSERT_EQ(result.size(), expected.size());

  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i].getType(), expected[i].getType());
    EXPECT_EQ(result[i].getValue(), expected[i].getValue());
  }
}