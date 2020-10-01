#ifndef AST_LOOP_NODE_H
#define AST_LOOP_NODE_H

#include "ASTNode.h"
#include "ASTBlock.h"

class ASTLoopNode : ASTNode {
    public:
        template<typename T, typename U>
        ASTLoopNode(T&&, U&&);
    private:
        ASTExpressionNode condition;
        ASTBlock block;

};

#endif
