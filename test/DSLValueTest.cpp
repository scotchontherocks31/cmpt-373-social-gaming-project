#include "DSLValue.h"
#include "gtest/gtest.h"
#include <json.hpp>

using namespace testing;
using namespace AST;
using Json = nlohmann::json;

TEST(DSLValueTest, Simple) {
  Json myJson = Json::parse("{\"Rounds\" : 10}");
  DSLValue actual(myJson);
  DSLValue expected = Map{{"Rounds", 10}};
  EXPECT_EQ(expected, myJson);
}

TEST(DSLValueTest, get) {
  DSLValue expected = Map{{"Rounds", 10}};
  Map &map = *expected.get<Map>();
  EXPECT_EQ(expected, map);
}
