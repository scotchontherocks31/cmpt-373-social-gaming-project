#ifndef AST_NODE_H
#define AST_NODE_H

#include <vector>

template <typename T>
class ASTVisitor;

class ASTNode {
    public:
        int numChildren() const;
        ASTNode& getParent() const;
        std::vector<ASTNode*> getChildren() const;
        virtual void accept(ASTVisitor<void>&) = 0;
        virtual ~ASTNode();
    protected:
        ASTNode* parent;
};

#endif
