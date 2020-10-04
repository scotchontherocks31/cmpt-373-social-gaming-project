#ifndef AST_NODE_H
#define AST_NODE_H

#include <vector>
#include <memory>

namespace AST {

class ASTNode {
    public:
        int numChildren() const {
            return numChildren;
        }
        // check mutability
        const std::vector<const ASTNode*> getChildren() const;
        const ASTNode& getParent() const;
        void setParent(ASTNode& parent);
        void accept(ASTVisitor& visitor);
        void setType(Type&);
        Type& getType() const;
        // Binding says variable available here scope (compile time)
        const Binding& getBinding() const;
        void setBinding(Binding&);
        virtual ~ASTNode();
    private:
        std::vector<std::unique_ptr<ASTNode>> children;
        ASTNode* parent;
        int numChildren;
};

}

#endif
