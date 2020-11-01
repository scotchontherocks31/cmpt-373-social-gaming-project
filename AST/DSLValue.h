#ifndef AST_DSLVALUE_H
#define AST_DSLVALUE_H

#include <json.hpp>
#include <map>
#include <random>
#include <set>
#include <variant>
#include <vector>

namespace AST {

class DSLValue;
using List = std::vector<DSLValue>;
using Map = std::map<std::string, DSLValue>;
using Json = nlohmann::json;

template <typename T>
concept BaseType =
    std::is_convertible<T, bool>::value ||
    std::is_convertible<T, std::string>::value ||
    std::is_convertible<T, int>::value || std::is_convertible<T, double>::value;

template <typename T>
concept DSLType = BaseType<T> || std::is_convertible<T, List>::value ||
                  std::is_convertible<T, Map>::value;

template <typename T> concept ListType = std::is_convertible<T, List>::value;

template <typename T> concept MapType = std::is_convertible<T, Map>::value;

template <typename T> concept ListOrMapType = ListType<T> || MapType<T>;

class DSLValue {
private:
  using InternalType =
      std::variant<std::monostate, bool, std::string, int, double,
                   std::shared_ptr<List>, std::shared_ptr<Map>>;
  InternalType value;

public:
  enum class Type { LIST, MAP, BOOLEAN, NUMBER, STRING, NIL };
  template <BaseType T>
  DSLValue(T &&value) noexcept : value{std::forward<T>(value)} {}
  DSLValue(const List &list) noexcept : value{std::make_shared<List>(list)} {}
  DSLValue(List &&list) noexcept
      : value{std::make_shared<List>(std::move(list))} {}
  DSLValue(const Map &map) noexcept : value{std::make_shared<Map>(map)} {}
  DSLValue(Map &&map) noexcept : value{std::make_shared<Map>(std::move(map))} {}
  DSLValue() noexcept = default;
  DSLValue(const Json &json) noexcept;
  DSLValue(const DSLValue &other) noexcept { value = other.value; }
  DSLValue(DSLValue &&other) noexcept { value = std::move(other.value); }
  template <BaseType T> T &get() noexcept { return std::get<T>(value); }
  template <ListOrMapType T> T &get() noexcept {
    return *std::get<std::shared_ptr<T>>(value);
  }
  template <BaseType T> const T &get() const noexcept {
    return std::get<T>(value);
  }
  template <ListOrMapType T> const T &get() const noexcept {
    return *std::get<std::shared_ptr<T>>(value);
  }
  template <BaseType T> DSLValue &operator=(T &&a) noexcept {
    value = std::forward<T>(a);
    return *this;
  }
  template <ListOrMapType T> DSLValue &operator=(T &&a) noexcept {
    value = std::make_shared<T>(std::forward<T>(a));
    return *this;
  }
  DSLValue &operator=(const DSLValue &other) noexcept {
    if (this != &other) {
      this->value = other.value;
    }
    return *this;
  }
  DSLValue &operator=(DSLValue &&other) noexcept {
    if (this != &other) {
      this->value = std::move(other.value);
    }
    return *this;
  }
  DSLValue &operator=(const Json &json) noexcept {
    *this = DSLValue{json};
    return *this;
  }
  const DSLValue &at(const std::string &key) const noexcept {
    const Map &map = get<Map>();
    return map.at(key);
  }
  DSLValue &operator[](const std::string &key) noexcept {
    Map &map = get<Map>();
    return map[key];
  }
  DSLValue &operator[](size_t index) noexcept {
    List &list = get<List>();
    return list[index];
  }
  List createKeyList(const std::string &key) noexcept {
    List returnList;
    Map map = get<Map>();
    std::ranges::transform(map, std::back_inserter(returnList),
                           [](auto &x) { return x.second; });
    return returnList;
  }
  List &extend(List &other) noexcept {
    List &list = get<List>();
    std::move(other.begin(), other.end(), std::back_inserter(other));
    return list;
  }
  List &reverse() noexcept {
    List &list = get<List>();
    std::ranges::reverse(list);
    return list;
  }
  List &shuffle() noexcept {
    List &list = get<List>();
    std::random_device rd;
    std::mt19937 generator(rd());
    std::ranges::shuffle(list, generator);
    return list;
  }
  Type getType() const noexcept;
  std::function<bool(const DSLValue &x, const DSLValue &y)>
  getCompareLambda(const DSLValue &value) const noexcept;
  List &sort() noexcept;
  List &sort(const std::string &key) noexcept;
  List &discard(size_t) noexcept;
  // TODO: Implement Deal and ask for specifications on the side effects of deal
};

} // namespace AST

#endif
