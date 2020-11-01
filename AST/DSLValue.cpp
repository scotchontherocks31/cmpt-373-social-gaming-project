#include "DSLValue.h"
#include <functional>

namespace {
using namespace AST;
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

bool listUnitype(List &list, auto &&convert) noexcept {
  if (list.size() == 0) {
    return true;
  }
  return std::ranges::all_of(list, [&](auto &x) {
    return convert(x).getType() == convert(list[0]).getType();
  });
}

List &sortChecker(DSLValue &dsl) noexcept {
  assert(dsl.getType() == DSLValue::Type::LIST);
  List &list = dsl.get<List>();
  auto identity = [](DSLValue &x) { return x; };
  if (!listUnitype(list, identity)) {
    assert("list not unitype");
    return list;
  }
  return list;
}

} // namespace

namespace AST {

std::function<bool(const DSLValue &x, const DSLValue &y)>
DSLValue::getCompareLambda(const DSLValue &value) const noexcept {
  using ReturnType = std::function<bool(const DSLValue &x, const DSLValue &y)>;
  return std::visit(
      overloaded{[](const std::string &discard) -> ReturnType {
                   return [](const DSLValue &x, const DSLValue &y) {
                     return x.get<std::string>() < y.get<std::string>();
                   };
                 },
                 [](const double &discard) -> ReturnType {
                   return [](const DSLValue &x, const DSLValue &y) {
                     return x.get<double>() < y.get<double>();
                   };
                 },
                 [](const int &discard) -> ReturnType {
                   return [](const DSLValue &x, const DSLValue &y) {
                     return x.get<double>() < y.get<double>();
                   };
                 },
                 [](const auto &invalid) -> ReturnType {
                   return [](const DSLValue &x, const DSLValue &y) {
                     return false;
                   };
                 }},
      value.value);
}

DSLValue::DSLValue(const Json &json) noexcept {
  if (json.is_null()) {
    value = std::monostate{};
  } else if (json.is_boolean()) {
    value = json.get<bool>();
  } else if (json.is_number()) {
    value = json.get<int>();
  } else if (json.is_string()) {
    value = json.get<std::string>();
  } else if (json.is_array()) {
    List list(static_cast<size_t>(json.size()));
    std::transform(json.begin(), json.end(), list.begin(),
                   [](const auto &x) { return DSLValue{x}; });
    value = std::make_shared<List>(std::move(list));
  } else if (json.is_object()) {
    Map map;
    std::transform(
        json.items().begin(), json.items().end(), std::inserter(map, map.end()),
        [](auto &x) { return std::make_pair(x.key(), DSLValue{x.value()}); });
    value = std::make_shared<Map>(std::move(map));
  } else {
    assert("json is of unknown type");
    value = std::monostate{};
  }
}

DSLValue::Type DSLValue::getType() const noexcept {
  return std::visit(
      overloaded{
          [](const std::shared_ptr<List> &discard) { return Type::LIST; },
          [](const std::shared_ptr<Map> &discard) { return Type::MAP; },
          [](const std::string &discard) { return Type::STRING; },
          [](const bool &discard) { return Type::BOOLEAN; },
          [](const double &list) { return Type::NUMBER; },
          [](const int &list) { return Type::NUMBER; },
          [](const std::monostate &list) { return Type::NIL; }},
      value);
}

List &DSLValue::sort() noexcept {
  auto &list = sortChecker(*this);
  if (list.size() == 0) {
    return list;
  }
  auto compare = getCompareLambda(list[0]);
  std::ranges::sort(list, compare);
  return list;
};

List &DSLValue::sort(const std::string &key) noexcept {
  auto &list = sortChecker(*this);
  if (list.size() == 0) {
    return list;
  }
  assert(list[0].getType() == Type::MAP);
  auto keyAvailable =
      std::all_of(list.begin(), list.end(), [&key](const DSLValue &dsl) {
        const Map &map = dsl.get<Map>();
        return map.contains(key);
      });
  assert(keyAvailable);
  auto convert = [&](const DSLValue &x) -> const DSLValue & {
    return x.at(key);
  };
  auto sameEmbeddedType = listUnitype(list, convert);
  assert(sameEmbeddedType);
  auto compare = getCompareLambda(convert(list[0]));
  std::ranges::sort(list, [&](const auto &x, const auto &y) {
    return compare(convert(x), convert(y));
  });
  return list;
}

List &DSLValue::discard(size_t i) noexcept {
  assert(getType() == DSLValue::Type::LIST);
  auto &list = get<List>();
  if (list.size() == 0) {
    return list;
  }
  if (i >= list.size()) {
    list.clear();
    return list;
  }
  list.erase(list.begin() + list.size() - i, list.end());
  return list;
}

} // namespace AST
