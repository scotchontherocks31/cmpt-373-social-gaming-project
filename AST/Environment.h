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

class Environment;

template <bool constant> class SymbolHandle {
private:
  friend Environment;
  Symbol *value;
  size_t depth;
  SymbolHandle(Symbol *value, size_t depth) : value{value}, depth{depth} {}

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
  size_t getDepth() const { return depth; }
};

using Handle = SymbolHandle<false>;
using ConstantHandle = SymbolHandle<true>;

template <typename T>
concept HandleType = std::same_as<std::remove_cvref_t<T>, Handle> ||
                     std::same_as<std::remove_cvref_t<T>, ConstantHandle>;

class Environment {
public:
  using Name = std::string;

private:
  const size_t depth;
  Environment *parent;
  std::map<Name, std::unique_ptr<Symbol>> allocatedSymbols;
  std::set<std::unique_ptr<Symbol>> rvalues;
  std::map<Name, Symbol *> bindings;
  std::variant<std::monostate, Handle, ConstantHandle, Symbol> returnRegister;
  Environment(Environment *parent) noexcept
      : depth{parent->depth + 1}, parent{parent} {}

  template <bool constant>
  std::optional<SymbolHandle<constant>> find(const Name &name) noexcept;

  template <bool constant>
  std::optional<SymbolHandle<constant>> getReturnValue() noexcept;

public:
  Environment() noexcept : depth{0}, parent{nullptr} {}
  Environment createChildEnvironment() noexcept;
  Environment(const Environment &) = delete;
  Environment(Environment &&) = delete;
  Environment &operator=(const Environment &) = delete;
  Environment &operator=(Environment &&) = delete;

  bool contains(const Name &name) noexcept;
  void removeBinding(const Name &name) noexcept;
  bool insertBinding(const Name &name, HandleType auto handle) noexcept;
  void insertOrAssignBinding(const Name &name, HandleType auto handle) noexcept;
  void allocate(const Name &name, SymbolType auto &&symbol) noexcept;
  std::optional<Handle> find(const Name &name) noexcept;
  std::optional<ConstantHandle> constFind(const Name &name) noexcept;

  std::optional<Handle> getReturnValue() noexcept;
  std::optional<ConstantHandle> getConstReturnValue() noexcept;
  void allocateReturn(SymbolType auto &&symbol) noexcept;
  void setReturn(HandleType auto handle) noexcept;
};

bool Environment::insertBinding(const Name &name,
                                HandleType auto handle) noexcept {
  if (depth < handle.getDepth()) {
    return false;
  }
  const auto &[it, success] =
      bindings.insert(std::make_pair(name, handle.value));
  return success;
}

void Environment::insertOrAssignBinding(const Name &name,
                                        HandleType auto handle) noexcept {
  if (depth < handle.getDepth()) {
    return;
  }
  bindings.insert_or_assign(name, handle.value);
}

void Environment::allocate(const Name &name,
                           SymbolType auto &&symbol) noexcept {
  removeBinding(name);
  const auto &[it, success] = allocatedSymbols.emplace(std::make_pair(
      name, std::make_unique<Symbol>(std::forward<decltype(symbol)>(symbol))));
  auto value = it->second.get();
  bindings.insert_or_assign(name, value);
}

void Environment::allocateReturn(SymbolType auto &&symbol) noexcept {
  if (not parent) {
    return;
  }
  parent->returnRegister = std::forward<decltype(symbol)>(symbol);
}

void Environment::setReturn(HandleType auto handle) noexcept {
  if (not parent) {
    return;
  }
  if (depth <= handle.getDepth()) {
    parent->returnRegister = std::monostate{};
  } else {
    parent->returnRegister = handle;
  }
}

template <bool constant>
std::optional<SymbolHandle<constant>>
Environment::find(const Environment::Name &name) noexcept {
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
  auto handle = SymbolHandle<constant>{symbol, env->depth};
  return std::optional<SymbolHandle<constant>>{handle};
}

template <bool constant>
std::optional<SymbolHandle<constant>> Environment::getReturnValue() noexcept {
  struct FetchReturnValue {
    Environment &env;
    FetchReturnValue(Environment &env) : env{env} {}
    using returnType = std::optional<SymbolHandle<constant>>;

    auto operator()(Symbol &symbol) noexcept -> returnType {
      if (symbol.constant and not constant) {
        return std::nullopt;
      }
      auto result =
          env.rvalues.emplace(std::make_unique<Symbol>(std::move(symbol)));
      return std::optional{
          SymbolHandle<constant>{result.first->get(), env.depth}};
    }
    auto operator()(Handle &handle) noexcept -> returnType {
      return std::optional{SymbolHandle<constant>{handle.value, handle.depth}};
    }
    auto operator()(ConstantHandle &handle) noexcept -> returnType {
      if (not constant) {
        return std::nullopt;
      }
      return std::optional{SymbolHandle<constant>{handle.value, handle.depth}};
    }
    auto operator()(std::monostate discard) noexcept -> returnType {
      return std::nullopt;
    }
  };
  auto result = std::visit(FetchReturnValue{*this}, returnRegister);
  returnRegister = std::monostate{};
  return result;
}

} // namespace AST

#endif
