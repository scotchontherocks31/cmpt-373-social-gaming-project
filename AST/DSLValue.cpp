#include "DSLValue.h"
#include <functional>
#include <iostream>
#include <ranges>

using namespace std;

namespace {
using namespace AST;

struct MutableAt {
  const string &key;
  explicit MutableAt(const string &key) noexcept : key{key} {};
  using returnType = optional<reference_wrapper<DSLValue>>;

  auto operator()(Map &map) noexcept -> returnType {
    auto it = map.find(key);
    return (it == map.end()) ? nullopt : returnType{it->second};
  }
  auto operator()(auto &&discard) noexcept -> returnType { return nullopt; }
};

struct At {
  const string &key;
  explicit At(const string &key) noexcept : key{key} {};
  using returnType = optional<reference_wrapper<const DSLValue>>;

  auto operator()(const Map &map) noexcept -> returnType {
    auto it = map.find(key);
    return (it == map.end()) ? nullopt : returnType{it->second};
  }
  auto operator()(const auto &discard) noexcept -> returnType {
    return nullopt;
  }
};

struct MutableListIndex {
  size_t index;
  explicit MutableListIndex(size_t index) : index{index} {};
  using returnType = optional<reference_wrapper<DSLValue>>;

  auto operator()(List &list) noexcept -> returnType {
    return index < list.size() ? returnType{list[index]} : nullopt;
  }
  auto operator()(auto &&discard) noexcept -> returnType { return nullopt; }
};

struct ListIndex {
  size_t index;
  explicit ListIndex(size_t index) : index{index} {};
  using returnType = optional<reference_wrapper<const DSLValue>>;

  auto operator()(const List &list) noexcept -> returnType {
    return index < list.size() ? returnType{list[index]} : nullopt;
  }
  auto operator()(const auto &discard) noexcept -> returnType {
    return nullopt;
  }
};

struct Size {
  explicit Size() = default;

  auto operator()(const Map &map) noexcept -> size_t { return map.size(); }
  auto operator()(const List &list) noexcept -> size_t { return list.size(); }
  auto operator()(const auto &discard) noexcept -> size_t { return 0; }
};

struct Slice {
  const string &key;
  explicit Slice(const string &key) : key{key} {};
  using returnType = optional<DSLValue>;

  auto operator()(const Map &map) noexcept -> returnType {
    auto it = map | ranges::views::filter([this](const auto &x) {
                return x.first == key;
              }) |
              ranges::views::transform([](const auto &x) { return x.second; });
    DSLValue returnValue = List{it.begin(), it.end()};
    return (returnValue.size() == map.size()) ? returnType{returnValue}
                                              : nullopt;
  }
  auto operator()(const auto &discard) noexcept -> returnType {
    return nullopt;
  }
};

struct Extend {
  explicit Extend() = default;

  auto operator()(List &to, List &from) noexcept {
    ranges::move(from, to.end());
  }
  auto operator()(auto &&x, auto &&y) noexcept { return; }
};

struct Reverse {
  explicit Reverse() = default;

  auto operator()(List &list) noexcept { ranges::reverse(list); }
  auto operator()(auto &&discard) noexcept { return; }
};

struct Shuffle {
  explicit Shuffle() = default;

  auto operator()(List &list) noexcept {
    std::random_device rd;
    std::mt19937 generate{rd()};
    ranges::shuffle(list, generate);
    return;
  }
  auto operator()(auto &&discard) noexcept { return; }
};

bool isUniType(const List &list) noexcept {
  if (list.size() <= 1) {
    return true;
  }
  const auto &firstElement = list[0];
  return ranges::all_of(list, [&firstElement](const auto &y) {
    return isSameType(firstElement, y);
  });
}

struct Sort {
  explicit Sort() = default;

  auto operator()(List &list) noexcept {
    if (not isUniType(list)) {
      return;
    }
  }
};

} // namespace

namespace AST {

optional<reference_wrapper<DSLValue>>
DSLValue::operator[](const string &key) noexcept {
  return unaryOperation(MutableAt{key});
}

optional<reference_wrapper<const DSLValue>>
DSLValue::at(const std::string &key) const noexcept {
  return unaryOperation(At(key));
}

optional<reference_wrapper<DSLValue>>
DSLValue::operator[](size_t index) noexcept {
  return unaryOperation(MutableListIndex{index});
}

optional<reference_wrapper<const DSLValue>>
DSLValue::operator[](size_t index) const noexcept {
  return unaryOperation(ListIndex{index});
}

optional<DSLValue>
DSLValue::createSlice(const std::string &key) const noexcept {
  return unaryOperation(Slice{key});
}

size_t DSLValue::size() const noexcept { return unaryOperation(Size{}); }

void extend(DSL auto &&to, DSL auto &&from) noexcept {
  to.binaryOperation(from, Extend{});
}

void reverse(DSL auto &&dsl) noexcept { dsl.unaryOperation(Reverse{}); }

void shuffle(DSL auto &&dsl) noexcept { dsl.unaryOperation(Shuffle{}); }

void sort(DSL auto &&dsl) noexcept { dsl.unaryOperation(Sort{}); }

} // namespace AST
