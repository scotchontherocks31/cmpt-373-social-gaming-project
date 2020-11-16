#include "Environment.h"
#include "gtest/gtest.h"
#include <optional>

using namespace testing;
using namespace AST;
using namespace std;

class EnvironmentTests : public Test {
protected:
  AST::Environment parent;
  Symbol symbol = Symbol{2, false};
  AST::Environment::Name key = "A";
  Symbol secondSymbol = Symbol{std::string{"Environment"}, true};
  AST::Environment::Name secondKey = "B";
  AST::Environment child1 = parent.createChildEnvironment();
  AST::Environment child2 = parent.createChildEnvironment();
  AST::Environment grandChild1 = child1.createChildEnvironment();
  AST::Environment grandChild2 = child2.createChildEnvironment();
};

TEST_F(EnvironmentTests, CreateChild) {
  auto child = parent.createChildEnvironment();
  child.allocate(key, symbol);
  EXPECT_EQ(symbol.dsl, child.find<false>(key));
  EXPECT_EQ(nullopt, parent.find<false>(key));
  auto secondChild = child.createChildEnvironment();
  secondChild.allocate(secondKey, secondSymbol);
  EXPECT_EQ(secondSymbol.dsl, secondChild.find<true>(secondKey));
  EXPECT_EQ(symbol.dsl, secondChild.find<false>(key));
  EXPECT_EQ(nullopt, secondChild.find<true>("Does not exist"));
}

TEST_F(EnvironmentTests, contains) {
  child1.allocate(key, symbol);
  child2.allocate(secondKey, secondSymbol);
  EXPECT_TRUE(child1.contains(key));
  EXPECT_TRUE(child2.contains(secondKey));
  EXPECT_FALSE(child1.contains(secondKey));
  EXPECT_FALSE(child2.contains(key));
  EXPECT_FALSE(parent.contains(key));
  EXPECT_FALSE(parent.contains(secondKey));
}

TEST_F(EnvironmentTests, removeBinding) {
  child1.allocate(key, symbol);
  grandChild1.allocate(secondKey, secondSymbol);
  EXPECT_TRUE(child1.contains(key));
  EXPECT_TRUE(grandChild1.contains(secondKey));
  child1.removeBinding(key);
  EXPECT_FALSE(child1.contains(key));
  EXPECT_TRUE(grandChild1.contains(secondKey));
  grandChild1.removeBinding(secondKey);
  EXPECT_FALSE(child1.contains(key));
  EXPECT_FALSE(grandChild1.contains(secondKey));
}

TEST_F(EnvironmentTests, find) {
  child1.allocate(key, symbol);
  grandChild1.allocate(secondKey, secondSymbol);
  child2.allocate(secondKey, symbol);
  grandChild2.allocate(key, secondSymbol);
  EXPECT_EQ(symbol.dsl, child1.find<true>(key));
  EXPECT_EQ(symbol.dsl, child1.find<false>(key));
  EXPECT_EQ(nullopt, child1.find<true>(secondKey));
  EXPECT_EQ(nullopt, child1.find<false>(secondKey));
  EXPECT_EQ(symbol.dsl, grandChild1.find<true>(key));
  EXPECT_EQ(symbol.dsl, grandChild1.find<false>(key));
  EXPECT_EQ(secondSymbol.dsl, grandChild1.find<true>(secondKey));
  EXPECT_EQ(nullopt, grandChild1.find<false>(secondKey));
  EXPECT_EQ(nullopt, parent.find<false>(key));
  EXPECT_EQ(nullopt, parent.find<true>(key));
  EXPECT_EQ(nullopt, parent.find<true>(secondKey));
  EXPECT_EQ(nullopt, parent.find<true>(secondKey));
  EXPECT_EQ(nullopt, child2.find<true>(key));
  EXPECT_EQ(nullopt, child2.find<false>(key));
  EXPECT_EQ(symbol.dsl, child2.find<true>(secondKey));
  EXPECT_EQ(symbol.dsl, child2.find<false>(secondKey));
  EXPECT_EQ(secondSymbol.dsl, grandChild2.find<true>(key));
  EXPECT_EQ(nullopt, grandChild2.find<false>(key));
  EXPECT_EQ(symbol.dsl, grandChild2.find<true>(secondKey));
  EXPECT_EQ(symbol.dsl, grandChild2.find<false>(secondKey));
}

TEST_F(EnvironmentTests, insertBinding) {
  child1.allocate(secondKey, secondSymbol);
  auto handle = child1.find<true>(secondKey);
  ;
  EXPECT_TRUE(handle);
  EXPECT_FALSE(child1.insertBinding(secondKey, *handle));
  EXPECT_TRUE(child1.contains(secondKey));
  EXPECT_TRUE(grandChild1.insertBinding(key, *handle));
  EXPECT_EQ(secondSymbol.dsl, grandChild1.find<true>(key));
  child1.allocate(key, symbol);
  auto handle2 = child1.find<true>(key);
  grandChild1.insertOrAssignBinding(key, *handle2);
  EXPECT_EQ(symbol.dsl, grandChild1.find<true>(key));
}

TEST_F(EnvironmentTests, Return) {
  parent.allocate(secondKey, secondSymbol);
  {
    auto grandChild1 = child1.createChildEnvironment();
    grandChild1.allocateReturn(symbol);
  }
  auto handle = child1.getReturnValue<false>();
  EXPECT_TRUE(handle);
  EXPECT_EQ(symbol.dsl, handle);
  {
    auto grandChild1 = child1.createChildEnvironment();
    auto handle = parent.find<true>(secondKey);
    EXPECT_TRUE(handle);
    grandChild1.setReturn(*handle);
  }
  auto handle2 = child1.getReturnValue<true>();
  EXPECT_TRUE(handle2);
  EXPECT_EQ(secondSymbol.dsl, handle2);
}
