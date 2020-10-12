#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "ASTNode.h"
#include <iostream>
#include <map>
#include <string>
#include <variant>

namespace AST {

class Communication {
public:
  void sendGlobalMessage(std::string &message) {
    std::cout << message << std::endl;
  }
};

class DSLValue;
using List = std::vector<DSLValue>;
using Map = std::map<std::string, DSLValue>;

template <typename T>
concept DSLType =
    std::is_convertible<T, bool>::value ||
    std::is_convertible<T, std::string>::value ||
    std::is_convertible<T, int>::value ||
    std::is_convertible<T, double>::value ||
    std::is_convertible<T, List>::value || std::is_convertible<T, Map>::value;

class DSLValue {
private:
  using InternalType =
      std::variant<std::monostate, bool, std::string, int, double, List, Map>;
  InternalType value;

public:
  template <DSLType T>
  DSLValue(T &&value) noexcept : value{std::forward<T>(value)} {}
  DSLValue() noexcept = default;
  DSLValue(const DSLValue &other) noexcept { this->value = other.value; }
  DSLValue(DSLValue &&other) noexcept { this->value = std::move(other.value); }
  template <DSLType T> T &get() { return std::get<T>(value); }
  template <DSLType T> auto &get_if() noexcept { return std::get_if<T>(value); }
  template <DSLType T> DSLValue &operator=(T &&a) noexcept {
    value = std::forward<T>(a);
    return *this;
  }
  DSLValue &operator=(const DSLValue &other) noexcept {
    this->value = other.value;
    return *this;
  }
  DSLValue &operator=(DSLValue &&other) noexcept {
    this->value = std::move(other.value);
    return *this;
  }
  DSLValue &operator[](const std::string &key) {
    Map &map = get<Map>();
    return map[key];
  }
  DSLValue &operator[](size_t index) {
    List &list = get<List>();
    return list[index];
  }
  List createKeyList(const std::string &key) {
    List returnList{};
    Map map = get<Map>();
    for (const auto &[x, y] : map) {
      returnList.push_back(y);
    }
    return returnList;
  }
};

class Environment {
public:
  using Lexeme = std::string;

private:
  Environment *parent;
  std::unique_ptr<Environment> child;
  std::map<Lexeme, DSLValue> bindings;

public:
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
    child = std::make_unique<Environment>(this);
    return *child;
  }
};

class ASTVisitor {
public:
  void visit(GlobalMessage &node) { visitHelper(node); }
  void visit(FormatNode &node) { visitHelper(node); }
  virtual ~ASTVisitor() = default;

private:
  virtual void visitHelper(GlobalMessage &) = 0;
  virtual void visitHelper(FormatNode &) = 0;
};

class Interpreter : public ASTVisitor {
public:
  Interpreter(Environment &&env, Communication &communication)
      : environment{std::move(env)}, communication{communication} {}

private:
  virtual void visitHelper(GlobalMessage &node) {
    visitEnter(node);
    node.acceptForChildren(*this);
    visitLeave(node);
  }
  virtual void visitHelper(FormatNode &node) {
    visitEnter(node);
    node.acceptForChildren(*this);
    visitLeave(node);
  }
  void visitEnter(GlobalMessage &node){};
  void visitLeave(GlobalMessage &node) {
    const auto &formatMessageNode = node.getFormatNode();
    auto &&formatMessage = formatMessageNode.getFormat();
    const std::string GAME_NAME = "Game Name";
    auto &&gameNameDSL = environment.getValue(GAME_NAME);
    auto &&gameName = gameNameDSL.get<std::string>();
    auto finalMessage = formatMessage + gameName;
    communication.sendGlobalMessage(finalMessage);
  };

  void visitEnter(FormatNode &node){};
  void visitLeave(FormatNode &node){};

private:
  Environment environment;
  Communication &communication;
};



class Printer : public ASTVisitor {    
    private:
        virtual void visitHelper(GlobalMessage& node) { 
            visitEnter(node);
            node.acceptForChildren(*this); 
        }
        virtual void visitHelper(FormatNode& node) { 
            visitEnter(node);
            node.acceptForChildren(*this); 
        }
        void visitEnter(GlobalMessage& node) {
            std::cout<<"GlobalMessage\n";                
        }; 
        void visitEnter(FormatNode& node) {   
            std::cout<<"FormatNode\n";
        };  
};


} // namespace AST
#endif
