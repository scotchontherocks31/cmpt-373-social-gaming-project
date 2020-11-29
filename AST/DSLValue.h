#ifndef AST_DSLVALUE_H
#define AST_DSLVALUE_H

#include <compare>
#include <json.hpp>
#include <map>
#include <optional>
#include <random>
#include <set>
#include <type_traits>
#include <variant>
#include <vector>

namespace AST {

using Json = nlohmann::json;

class DSLValue;

using List = std::vector<DSLValue>;
using Map = std::map<std::string, DSLValue>;
struct Nil {};

template <typename T>
concept BaseType =
    std::is_same_v<std::remove_cvref_t<T>, bool> ||
    std::is_same_v<std::remove_cvref_t<T>, std::string> ||
    std::is_same_v<std::remove_cvref_t<T>, int> || std::is_same_v<std::remove_cvref_t<T>, double>;

template <typename T>
concept DSLType = BaseType<T> || std::is_same_v<std::remove_cvref_t<T>, List> ||
                  std::is_same_v<std::remove_cvref_t<T>, Map>;

template <typename T> concept DSL = std::is_same_v<DSLValue, std::remove_cvref_t<T>>;

template <typename F, typename... Types>
concept BoundedUnaryOperation = requires(F &&f, Types &&...types) {
  (std::invoke(std::forward<F>(f), std::forward<Types>(types)), ...);
};

template <typename F, typename Type1, typename... Types2>
requires requires(F &&f, Type1 &&type, Types2 &&...types) {
  (std::invoke(std::forward<F>(f), std::forward<Type1>(type), std::forward<Types2>(types)), ...);
}
constexpr inline void NestedApply(F &&f, Type1 &&type, Types2 &&...types) { return; }

template <typename F, typename... Types1>
concept BoundedSymmetricBinaryOperation = requires(F &&f, Types1 &&...types1, Types1 &&...types2) {
  (NestedApply(f, std::forward<Types1>(types1), std::forward<Types1>(types2)...), ...);
};

template <typename F>
concept UnaryDSLOperation =
    BoundedUnaryOperation<F, std::monostate, bool, int, double, std::string, List, Map>;

template <typename F>
concept BinaryDSLOperation =
    BoundedSymmetricBinaryOperation<F, std::monostate, bool, int, double, std::string, List, Map>;

class DSLValue {
private:
  using InternalType = std::variant<std::monostate, bool, std::string, int, double, List, Map>;
  InternalType value;

public:
  enum class Type { LIST, MAP, BOOLEAN, NUMBER, STRING, NIL };

  // Constructors
  DSLValue() noexcept = default;
  DSLValue(const Json &json) noexcept;
  template <DSLType T> DSLValue(T &&value) noexcept : value{std::forward<T>(value)} {};
  DSLValue(const DSLValue &other) noexcept { value = other.value; }
  DSLValue(DSLValue &&other) noexcept { value = std::move(other.value); }
  template <DSLType T> DSLValue &operator=(T &&a) noexcept {
    value = std::forward<T>(a);
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

  template <typename T> std::optional<std::reference_wrapper<T>> get() noexcept {
    auto pointerToValue = std::get_if<T>(&value);
    if (not pointerToValue) {
      return std::nullopt;
    }
    return std::optional{std::reference_wrapper<T>{*pointerToValue}};
  }

  template <UnaryDSLOperation F> decltype(auto) unaryOperation(F &&f) {
    return std::visit(f, value);
  }

  template <UnaryDSLOperation F> decltype(auto) unaryOperation(F &&f) const {
    return std::visit(f, value);
  }

  template <DSL U, BinaryDSLOperation F> decltype(auto) binaryOperation(U &&other, F &&f) {
    return std::visit(f, value, other.value);
  }

  template <DSL U, BinaryDSLOperation F>
  decltype(auto) binaryOperation(const U &other, F &&f) const {
    return std::visit(f, value, other.value);
  }

  std::optional<std::reference_wrapper<const DSLValue>> at(const std::string &key) const noexcept;

  std::optional<std::reference_wrapper<DSLValue>> operator[](const std::string &key) noexcept;

  std::optional<std::reference_wrapper<DSLValue>> operator[](size_t index) noexcept;

  std::optional<std::reference_wrapper<const DSLValue>> operator[](size_t index) const noexcept;

  std::optional<DSLValue> createSlice(const std::string &key) const noexcept;
  size_t size() const noexcept;

  friend std::partial_ordering operator<=>(const DSLValue &x, const DSLValue &y) noexcept = default;
};

bool isSortableType(const DSLValue &x) noexcept;
bool isSameType(const DSLValue &x, const DSLValue &y) noexcept;
void extend(DSL auto &&to, DSL auto &&from) noexcept;
void reverse(DSL auto &&x) noexcept;
void shuffle(DSL auto &&x) noexcept;
void sort(DSL auto &&x) noexcept;
void sort(DSL auto &&x, const std::string &key) noexcept;
void discard(DSL auto &&x, size_t count) noexcept;
void deal(DSL auto &&from, DSL auto &&to, size_t count) noexcept;
std::ostream &operator<<(std::ostream &os, const DSLValue &x) noexcept;

} // namespace AST

#endif
