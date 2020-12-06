#include "DSLValue.h"
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>

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

bool isUniType(const List &list) noexcept {
  if (list.size() <= 1) {
    return true;
  }
  const auto &firstElement = list[0];
  return ranges::all_of(list, [&firstElement](const auto &y) {
    return isSameType(firstElement, y);
  });
}


struct Slice {
  const string &key;
  explicit Slice(const string &key) : key{key} {};
  using returnType = optional<DSLValue>;

  auto operator()(const List &list) noexcept -> returnType {
    if (not (isUniType(list) and typeCheck(list.front(), DSLValue::Type::MAP))) {
      return nullopt;
    }
    auto it = list | ranges::views::filter([this](const auto &x) {
                return x.at(key).has_value();
              }) |
              ranges::views::transform([this](const auto &x) { return *x.at(key); });
    DSLValue returnValue = List{it.begin(), it.end()};
    return (returnValue.size() == list.size()) ? returnType{returnValue}
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
    from.clear();
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

bool isSortableList(const List &list) noexcept {
  if (list.size() <= 1) {
    return false;
  }
  const auto &firstElement = list[0];
  return isSortableType(firstElement);
}

struct Sort {
  explicit Sort() = default;

  auto operator()(List &list) noexcept {
    if (not isUniType(list) or not isSortableList(list)) {
      return;
    }
    ranges::sort(list);
  }
  auto operator()(auto &&discard) noexcept { return; }
};

struct SameType {
  explicit SameType() = default;

  template <typename T> auto operator()(const T &x, const T &y) noexcept {
    return true;
  }
  auto operator()(const auto &x, const auto &y) noexcept { return false; }
};

struct SortableType {
  explicit SortableType() = default;

  auto operator()(const Map &map) noexcept { return false; }
  auto operator()(const List &list) noexcept { return false; }
  auto operator()(const monostate &mono) noexcept { return false; }
  auto operator()(const auto &discard) noexcept { return true; }
};

bool keysExist(const List &list, const std::string &key) {
  auto it = list | ranges::views::transform(
                       [&key](const auto &x) { return x.at(key); });
  return all_of(it.begin(), it.end(),
                [](const auto &x) { return x.has_value(); });
}

struct SortWithKey {
  const string &key;
  explicit SortWithKey(const string &key) noexcept : key{key} {}

  auto operator()(List &list) noexcept {
    if (not isUniType(list) or not keysExist(list, key)) {
      return;
    }
    ranges::sort(list, [this](const auto &x, const auto &y) {
      return *(x.at(key)) < *(y.at(key));
    });
  }
  auto operator()(auto &&discard) noexcept { return; }
};

struct Discard {
  size_t count;
  explicit Discard(size_t count) noexcept : count{count} {};

  auto operator()(List &list) noexcept {
    if (count == 0) {
      return;
    }
    count = (count >= list.size()) ? list.size() : count;
    list.erase(list.end() - count, list.end());
  }
  auto operator()(auto &&discard) noexcept { return; }
};

struct Deal {
  size_t count;
  explicit Deal(size_t count) noexcept : count{count} {};

  auto operator()(List &to, List &from) noexcept {
    if (count == 0) {
      return;
    }
    count = (count >= from.size()) ? from.size() : count;
    move(from.end() - count, from.end(), to.end());
    from.erase(from.end() - count, from.end());
  }
  auto operator()(auto &&x, auto &&y) noexcept { return; }
};

ostream &join(auto begin, auto end, ostream &os, string separator,
              auto transform) {
  if (begin != end) {
    os << transform(*begin);
    ++begin;
  }
  for (; begin != end; ++begin) {
    os << separator << transform(*begin);
  }
  return os;
}

struct Print {
  ostream &os;
  explicit Print(ostream &os) : os{os} {}

  auto operator()(const Map &map) -> ostream & {
    os << "{";
    join(map.begin(), map.end(), os, ", ", [](const auto &x) {
      stringstream ss;
      ss << x.first << ": " << x.second;
      return ss.str();
    });
    os << "}";
    return os;
  }
  auto operator()(const List &list) -> ostream & {
    os << "[";
    join(list.begin(), list.end(), os, ", ", [](const auto &x) { return x; });
    os << "]";
    return os;
  }
  auto operator()(monostate x) -> ostream & {
    os << "nil";
    return os;
  }
  auto operator()(const string &x) -> ostream & {
    os << "\"" << x << "\"";
    return os;
  }
  auto operator()(const auto &x) -> ostream & {
    os << x;
    return os;
  }
};

struct TypeCheck {
  const DSLValue::Type type;
  explicit TypeCheck(DSLValue::Type type) noexcept : type{type} {}

  auto operator()(const Map &map) noexcept -> bool {
    return type == DSLValue::Type::MAP;
  }
  auto operator()(const List &list) noexcept -> bool {
    return type == DSLValue::Type::LIST;
  }
  auto operator()(double x) noexcept -> bool {
    return type == DSLValue::Type::DOUBLE;
  }
  auto operator()(int x) noexcept-> bool {
    return type == DSLValue::Type::INT;
  }
  auto operator()(bool x) noexcept -> bool {
    return type == DSLValue::Type::BOOLEAN;
  }
  auto operator()(const std::string &x) noexcept -> bool {
    return type == DSLValue::Type::STRING;
  }
  auto operator()(std::monostate m) noexcept -> bool {
    return type == DSLValue::Type::NIL;
  }
};

struct Not {
  explicit Not() noexcept = default;

  auto operator()(bool &x) noexcept {
    x = not x;
  }
  auto operator()(auto &&discard) noexcept {
  }
};

struct Equal {
  explicit Equal() noexcept = default;
  using returnType = std::optional<bool>;

  auto operator()(double x, double y) noexcept -> returnType {
    return std::optional<bool>{x == y};
  }
  auto operator()(int x, int y) noexcept -> returnType {
    return std::optional<bool>{x == y};
  }
  auto operator()(int x, double y) noexcept -> returnType {
    return std::optional<bool>{x == y};
  }
  auto operator()(double x, int y) noexcept -> returnType {
    return std::optional<bool>{x == y};
  }
  auto operator()(bool x, bool y) noexcept -> returnType {
    return std::optional<bool>{x == y};
  }
  auto operator()(const string &x, const string &y) noexcept -> returnType {
    return std::optional<bool>{x == y};
  }
  auto operator()(const auto &x, const auto &y) noexcept -> returnType {
    return std::nullopt;
  }
};

struct Smaller {
  explicit Smaller() noexcept = default;
  using returnType = std::optional<bool>;

  auto operator()(double x, double y) noexcept -> returnType {
    return std::optional<bool>{x < y};
  }
  auto operator()(int x, int y) noexcept -> returnType {
    return std::optional<bool>{x < y};
  }
  auto operator()(int x, double y) noexcept -> returnType {
    return std::optional<bool>{x < y};
  }
  auto operator()(double x, int y) noexcept -> returnType {
    return std::optional<bool>{x < y};
  }
  auto operator()(const string &x, const string &y) noexcept -> returnType {
    return std::optional<bool>{x < y};
  }
  auto operator()(const auto &x, const auto &y) noexcept -> returnType {
    return std::nullopt;
  }
};

struct Greater {
  explicit Greater() noexcept = default;
  using returnType = std::optional<bool>;

  auto operator()(double x, double y) noexcept -> returnType {
    return std::optional<bool>{x > y};
  }
  auto operator()(int x, int y) noexcept -> returnType {
    return std::optional<bool>{x > y};
  }
  auto operator()(int x, double y) noexcept -> returnType {
    return std::optional<bool>{x > y};
  }
  auto operator()(double x, int y) noexcept -> returnType {
    return std::optional<bool>{x > y};
  }
  auto operator()(const string &x, const string &y) noexcept -> returnType {
    return std::optional<bool>{x > y};
  }
  auto operator()(const auto &x, const auto &y) noexcept -> returnType {
    return std::nullopt;
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
void reverse(DSL auto &&x) noexcept { x.unaryOperation(Reverse{}); }
void shuffle(DSL auto &&x) noexcept { x.unaryOperation(Shuffle{}); }
void sort(DSL auto &&x) noexcept { x.unaryOperation(Sort{}); }
void sort(DSL auto &&x, const std::string &key) noexcept {
  x.unaryOperation(SortWithKey{key});
}
bool isSameType(const DSLValue &x, const DSLValue &y) noexcept {
  return x.binaryOperation(y, SameType{});
}
bool isSortableType(const DSLValue &x) noexcept {
  return x.unaryOperation(SortableType{});
}
void discard(DSL auto &&x, size_t count) noexcept {
  return x.unaryOperation(Discard{count});
}
void deal(DSL auto &&from, DSL auto &&to, size_t count) noexcept {
  return from.binaryOperation(to, Deal{count});
}
ostream &operator<<(ostream &os, const DSLValue &x) noexcept {
  return x.unaryOperation(Print{os});
}
DSLValue::DSLValue(const Json &json) noexcept {
  if (json.is_null()) {
    value = std::monostate{};
  } else if (json.is_boolean()) {
    value = json.get<bool>();
  } else if (json.is_number_integer()) {
    value = json.get<int>();
  } else if (json.is_number_float()) {
    value = json.get<double>();
  } else if (json.is_string()) {
    value = json.get<std::string>();
  } else if (json.is_array()) {
    List list;
    list.reserve(static_cast<size_t>(json.size()));
    std::transform(json.begin(), json.end(), back_inserter(list),
                   [](const auto &x) { return DSLValue{x}; });
    value = std::move(list);
  } else if (json.is_object()) {
    Map map;
    std::transform(
        json.items().begin(), json.items().end(), std::inserter(map, map.end()),
        [](auto &x) { return std::make_pair(x.key(), DSLValue{x.value()}); });
    value = std::move(map);
  } else {
    assert("json is of unknown type");
    value = std::monostate{};
  }
}
bool typeCheck(const DSLValue &x, DSLValue::Type type) noexcept {
  return x.unaryOperation(TypeCheck{type});
}
std::optional<bool> 
equal(const DSLValue &x, const DSLValue &y) noexcept {
  return x.binaryOperation(y, Equal{});
}
std::optional<bool> 
greater(const DSLValue &x, const DSLValue &y) noexcept {
  return x.binaryOperation(y, Greater{});
}
std::optional<bool> 
smaller(const DSLValue &x, const DSLValue &y) noexcept {
  return x.binaryOperation(y, Smaller{});
}

} // namespace AST
