#include "gtest/gtest.h"
#include "ExpressionParser.h"
#include <vector>
using namespace testing;

TEST(ExpressionParseTest, initial) {
    std::vector<std::string> expected;
    std::string str = "winners.size.big.size == winners.size.small.fast";
    expected.push_back("winners.size.big.size");
    expected.push_back("winners.size.small.fast");

    auto result = parseExpression(str);
    //EXPECT_EQ(str, expected);
}
