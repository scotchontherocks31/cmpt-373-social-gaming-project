#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include <string>
#include <map>
#include <iostream>
#include <variant>
#include "ASTNode.h"

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

class None{};

template <typename T>
concept DSLType = std::is_convertible<T, bool>::value ||
                  std::is_convertible<T, std::string>::value ||
                  std::is_convertible<T, int>::value ||
                  std::is_convertible<T, double>::value ||
                  std::is_convertible<T, List>::value ||
                  std::is_convertible<T, Map>::value ||
                  std::is_convertible<T, None>::value;

class DSLValue {
    private:
        using InternalType = std::variant<None, bool, std::string, int, double, List, Map>;
        InternalType value;
    public:
        template <DSLType T>
        explicit DSLValue(T&& value) : value{std::forward<T>(value)} {}
        DSLValue() : value{None{}} {}
        DSLValue(const DSLValue &other) noexcept {
            this->value = other.value;
        }
        DSLValue(DSLValue &&other) noexcept {
            this->value = std::move(other.value);
        }
        template <DSLType T>
        T& get() {
            return std::get<T>(value);
        }
        template <DSLType T>
        auto& get_if() noexcept {
            return std::get_if<T>(value);
        }
        template <DSLType T> 
        DSLValue& operator=(T &&a) noexcept {
            value = std::forward<T>(a);
            return *this;
        }
        DSLValue& operator=(const DSLValue &other) noexcept {
            this->value = other.value;
            return *this;
        }
        DSLValue& operator=(DSLValue &&other) noexcept {
            this->value = std::move(other.value);
            return *this;
        }
        DSLValue& operator[](const std::string &key) {
            Map &map = get<Map>();
            return map[key];
        }
        DSLValue& operator[](size_t index) {
            List &list = get<List>();
            return list[index];
        }
        List createKeyList(const std::string &key) {
            List returnList{};
            Map map = get<Map>();
            for (const auto& [x, y] : map) {
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
        DSLValue& getValue(const Lexeme &lexeme) noexcept {
            return bindings[lexeme];
        }
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
        Environment& createChildEnvironment() noexcept {
            child = std::make_unique<Environment>(this);
            return *child;
        }
};

class ASTVisitor {
    public:
        void visit(GlobalMessage& node) { visitHelper(node); }
        virtual ~ASTVisitor() = 0;
    private:
        virtual void visitHelper(GlobalMessage&) = 0;
};


class Interpreter : public ASTVisitor {
    public:
        Interpreter(Environment&& env, Communication &communication) : 
            environment{std::move(env)}, communication{communication} {}
    private:
        virtual void visitHelper(GlobalMessage& node) { 
            visitEnter(node);
            node.acceptForChildren(*this); 
            visitLeave(node);
        }
        void visitEnter(GlobalMessage& node) {};
        void visitLeave(GlobalMessage& node) {};
    private:
        Environment environment;
        Communication &communication;
};

}
#endif
