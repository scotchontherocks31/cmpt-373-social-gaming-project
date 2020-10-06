#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include <string>
#include <unordered_map>
#include "ASTNode.h"

namespace AST {

class Communication {
    public:
        void sendGlobalMessage(std::string&);
};

class DSLValue {
    int a;
};

class Environment {
    public:
        const DSLValue& getValue(ASTNode&) const;
        void removeBinding(ASTNode&);
        bool constains(ASTNode&);
        void setBinding(ASTNode&, DSLValue&);
        Environment& createChildEnvironment();
        void removeChildEnvironment();
    private:
        Environment *parent;
        std::unique_ptr<Environment> child;
        std::unordered_map<ASTNode*, DSLValue> bindings;
};

class ASTVisitor {
    public:

        void visit(GlobalMessage& node) {
            visitHelper(node);
        }
        virtual ~ASTVisitor() = 0;
    private:
//        virtual void visitLeave(ForEachNode&) = 0;
//        virtual void visitEnter(ForEachNode&) = 0;
        virtual void visitHelper(GlobalMessage&) = 0;
//        virtual void visitLeave(ListNode&);
//        virtual void visitEnter(ListNode&);
//        virtual void visitLeave(ElementNode&);
//        virtual void visitEnter(ElementNode&);
//        virtual void visitLeave(RulesNode&);
//        virtual void visitEnter(RulesNode&);
//        virtual void visitLeave(FormatNode&);
//        virtual void visitEnter(FormatNode&);
};


class Interpreter : public ASTVisitor {
    public:
        Interpreter(Environment&& env, Communication &communication) : 
            environment{std::move(env)}, communication{communication} {}
    private:
        virtual void visitHelper(GlobalMessage& node) {
            node.acceptForChildren(*this);
        }
//        virtual void visitLeave(ForEachNode&) override;
//        virtual void visitEnter(ForEachNode& node) override;
//        virtual void visitLeave(GlobalMessageNode&) override;
//        virtual void visitEnter(GlobalMessageNode&) override;
//        virtual void visitLeave(ListNode&) override;
//        virtual void visitEnter(ListNode&) override;
//        virtual void visitLeave(ElementNode&) override;
//        virtual void visitEnter(ElementNode&) override;
//        virtual void visitLeave(RulesNode&) override;
//        virtual void visitEnter(RulesNode&) override;
//        virtual void visitLeave(FormatNode&) override;
//        virtual void visitEnter(FormatNode&) override;
        Environment environment;
        Communication &communication;
};

}
#endif
