#include "ExpressionParser.h"
#include "gtest/gtest.h"
#include <vector>
using namespace testing;

TEST(ExpressionParseTest, simpleDot) {
  std::string str = "winners.size";

  std::vector<std::string> expected;
  expected.push_back("winners.size");

  std::vector<std::string> result = parseExpression(str);

  ASSERT_EQ(result.size(), expected.size());
  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i], expected[i]);
  }
}

TEST(ExpressionParseTest, chainedDot) {
  std::string str = "Bobby.Name.hello.bye.sad";

  std::vector<std::string> expected;
  expected.push_back("Bobby.Name.hello.bye.sad");

  std::vector<std::string> result = parseExpression(str);

  ASSERT_EQ(result.size(), expected.size());
  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i], expected[i]);
  }
}

TEST(ExpressionParseTest, ChainedDotSimpleEquals) {
  std::string str = "winners.size.big.size == winners.size.small.fast";

  std::vector<std::string> expected;
  expected.push_back("winners.size.big.size ");
  expected.push_back("==");
  expected.push_back("winners.size.small.fast");

  std::vector<std::string> result = parseExpression(str);

  ASSERT_EQ(result.size(), expected.size());
  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i], expected[i]);
  }
}

TEST(ExpressionParseTest, ChainedDotChainedEquals) {
  std::string input =
      "(winners.size.big.size == winners.size.small.fast) == true";
  std::vector<std::string> result;

  std::vector<std::string> expected;
  expected.push_back("winners.size.big.size ");
  expected.push_back("==");
  expected.push_back("winners.size.small.fast");
  expected.push_back("==");
  expected.push_back("true");

  result = parseExpression(input);

  ASSERT_EQ(result.size(), expected.size());
  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(result[i], expected[i]);
  }
}
