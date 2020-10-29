#pragma once
#include "ASTNode.h"
#include <json.hpp>

using DSLValue = nlohmann::json;

namespace AST {

class Environment {
public:
  using Lexeme = std::string;

private:
  Environment *parent;
  std::vector<Environment> children;
  std::map<Lexeme, DSLValue> bindings;

public:
  Environment() : parent{nullptr} {}
  explicit Environment(Environment *parent) : parent{parent} {}
  DSLValue &getValue(const Lexeme &lexeme) noexcept { return bindings[lexeme]; }
  void removeBinding(const Lexeme &lexeme) noexcept {
    if (bindings.contains(lexeme)) {
      bindings.erase(lexeme);
    }
  }
  bool contains(const Lexeme &lexeme) noexcept {
    return bindings.contains(lexeme);
  }
  void setBinding(const Lexeme &lexeme, DSLValue value) noexcept {
    bindings.insert_or_assign(lexeme, std::move(value));
  }
  Environment &createChildEnvironment() noexcept {
    this->children.emplace_back(this);
    return this->children.back();
  }
};

} // namespace AST
