#include "Environment.h"

namespace AST {

Environment Environment::createChildEnvironment() noexcept {
  return Environment{this};
}

bool Environment::contains(const Name &name) noexcept {
  return bindings.contains(name);
}

void Environment::removeBinding(const Name &name) noexcept {
  if (not contains(name)) {
    return;
  }
  auto node = bindings.extract(name);
  allocatedSymbols.extract(name);
}

std::optional<Handle> Environment::find(const Name &name) noexcept {
  return find<false>(name);
}

std::optional<ConstantHandle>
Environment::constFind(const Name &name) noexcept {
  return find<true>(name);
}

std::optional<Handle> Environment::getReturnValue() noexcept {
  return getReturnValue<false>();
}

std::optional<ConstantHandle> Environment::getConstReturnValue() noexcept {
  return getReturnValue<true>();
}

} // namespace AST
