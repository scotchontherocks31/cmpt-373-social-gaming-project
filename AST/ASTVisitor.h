#ifndef AST_VISITOR_H
#define AST_VISITOR_H

namespace AST {

class Communication {
    public:
        sendGlobalMessage(std::string&);
};

class ASTVisitor {
    public:
        virtual void visitLeave(ForEachNode&);
        virtual void visitEnter(ForEachNode&);
        virtual void visitLeave(GlobalMessageNode&);
        virtual void visitEnter(GlobalMessageNode&);
        virtual void visitLeave(ListNode&);
        virtual void visitEnter(ListNode&);
        virtual void visitLeave(ElementNode&);
        virtual void visitEnter(ElementNode&);
        virtual void visitLeave(RulesNode&);
        virtual void visitEnter(RulesNode&);
        virtual void visitLeave(FormatNode&);
        virtual void visitEnter(FormatNode&);
        virtual ~ASTVisitor() = 0;
    private:
};

class Interpreter : public ASTVisitor {
    public:
        virtual void visitLeave(ForEachNode&) override;
        virtual void visitEnter(ForEachNode& node) override;
        virtual void visitLeave(GlobalMessageNode&) override;
        virtual void visitEnter(GlobalMessageNode&) override;
        virtual void visitLeave(ListNode&) override;
        virtual void visitEnter(ListNode&) override;
        virtual void visitLeave(ElementNode&) override;
        virtual void visitEnter(ElementNode&) override;
        virtual void visitLeave(RulesNode&) override;
        virtual void visitEnter(RulesNode&) override;
        virtual void visitLeave(FormatNode&) override;
        virtual void visitEnter(FormatNode&) override;
        private:
        Environment environment;
        Communication &communication;
};

class DSLValue;

class Environment {
    public:
        const DSLValue& getValue(ASTNode&) const;
        void removeBinding(ASTNode&);
        bool constains(ASTNode&);
        void setBinding(ASTNode&, DSLValue);
        Environment& createChildEnvironment();
        void removeChildEnvironment();
    private:
        Environment *parent;
        std::unique_ptr<Environment> child;
        std::unordered_map<ASTNode*, DSLValue> bindings;
};


}
#endif
