#ifndef AST_ENVIRONMENT_H
#define AST_ENVIRONMENT_H

#include "DSLValue.h"
#include <map>
#include <memory>
#include <set>

namespace AST {

struct Symbol {
  DSLValue dsl;
  bool constant = false;
};

template <typename T>
concept SymbolType = std::same_as<std::remove_cvref_t<T>, Symbol>;

class Environment {
public:
  using Name = std::string;

private:
  Environment *parent;
  std::map<Name, std::unique_ptr<Symbol>> allocatedSymbols;
  std::set<std::unique_ptr<Symbol>> rvalues;
  std::map<Name, Symbol *> bindings;
  std::variant<std::monostate, Symbol *, Symbol> returnRegister;
  Environment(Environment *parent) noexcept : parent{parent} {}

public:
  template <bool constant> class SymbolHandle;

  Environment() noexcept : parent{nullptr} {}
  Environment createChildEnvironment() noexcept;
  Environment(const Environment &) = delete;
  Environment(Environment &&) = delete;
  Environment &operator=(const Environment &) = delete;
  Environment &operator=(Environment &&) = delete;

  bool contains(const Name &name) noexcept;
  void removeBinding(const Name &name) noexcept;
  template <bool constant>
  bool insertBinding(const Name &name, SymbolHandle<constant> handle) noexcept;
  template <bool constant>
  void insertOrAssignBinding(const Name &name,
                             SymbolHandle<constant> handle) noexcept;
  void allocate(const Name &name, SymbolType auto &&symbol) noexcept;
  template <bool constant>
  std::optional<SymbolHandle<constant>> find(const Name &name) noexcept;

  template <bool constant>
  std::optional<SymbolHandle<constant>> getReturnValue() noexcept;
  void allocateReturn(SymbolType auto &&symbol) noexcept;
  template <bool constant>
  void setReturn(SymbolHandle<constant> handle) noexcept;
};

template <bool constant> class Environment::SymbolHandle {
private:
  friend Environment;
  SymbolHandle(Symbol *value) : value{value} {}
  Symbol *value;

public:
  SymbolHandle(const SymbolHandle &other) = default;
  SymbolHandle(SymbolHandle &&other) = default;
  SymbolHandle &operator=(const SymbolHandle &other) = default;
  SymbolHandle &operator=(SymbolHandle &&other) = default;
  operator DSLValue &() const requires(not constant) {
    assert(value);
    return value->dsl;
  }
  operator const DSLValue &() const {
    assert(value);
    return value->dsl;
  }
  auto subsetDSL(auto f) const { return SymbolHandle{f(*value)}; };
  bool isConstant() const { return value->constant; }
};

template <bool constant>
bool Environment::insertBinding(const Name &name,
                                SymbolHandle<constant> handle) noexcept {
  auto result = bindings.insert(std::make_pair(name, handle.value));
  return result.second;
}

template <bool constant>
void Environment::insertOrAssignBinding(
    const Name &name, SymbolHandle<constant> handle) noexcept {
  bindings.insert_or_assign(name, handle.value);
}

void Environment::allocate(const Name &name,
                           SymbolType auto &&symbol) noexcept {
  removeBinding(name);
  auto result = allocatedSymbols.emplace(std::make_pair(
      name, std::make_unique<Symbol>(std::forward<decltype(symbol)>(symbol))));
  auto value = (result.first)->second.get();
  bindings.insert_or_assign(name, value);
}

template <bool constant>
std::optional<Environment::SymbolHandle<constant>>
Environment::find(const Name &name) noexcept {
  Environment *env = this;
  while (env) {
    if (env->contains(name)) {
      break;
    }
    env = env->parent;
  }
  auto symbol = env ? env->bindings[name] : nullptr;
  if (not symbol or (symbol->constant and not constant)) {
    return std::nullopt;
  }
  SymbolHandle<constant> handle{symbol};
  return std::optional<Environment::SymbolHandle<constant>>{handle};
}

template <bool constant>
std::optional<Environment::SymbolHandle<constant>>
Environment::getReturnValue() noexcept {
  struct FetchReturnValue {
    Environment &env;
    FetchReturnValue(Environment &env) : env{env} {}
    using returnType = std::optional<Environment::SymbolHandle<constant>>;

    auto operator()(Symbol &symbol) noexcept -> returnType {
      if (symbol.constant and not constant) {
        return std::nullopt;
      }
      auto result =
          env.rvalues.emplace(std::make_unique<Symbol>(std::move(symbol)));
      return std::optional{SymbolHandle<constant>{result.first->get()}};
    }
    auto operator()(Symbol *symbol) noexcept -> returnType {
      if (symbol->constant and not constant) {
        return std::nullopt;
      }
      return std::optional{SymbolHandle<constant>{symbol}};
    }
    auto operator()(std::monostate discard) noexcept -> returnType {
      return std::nullopt;
    }
  };
  auto result = std::visit(FetchReturnValue{*this}, returnRegister);
  returnRegister = std::monostate{};
  return result;
}

void Environment::allocateReturn(SymbolType auto &&symbol) noexcept {
  if (not parent) {
    return;
  }
  parent->returnRegister = std::forward<decltype(symbol)>(symbol);
}

template <bool constant>
void Environment::setReturn(SymbolHandle<constant> handle) noexcept {
  if (not parent) {
    return;
  }
  parent->returnRegister = handle.value;
}

} // namespace AST

#endif
