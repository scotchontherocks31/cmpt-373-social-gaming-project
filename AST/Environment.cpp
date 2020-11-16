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

} // namespace AST
