#ifndef AST_EXPRESSION_NODE_H
#define AST_EXPRESSION_NODE_H

#include "ASTNode.h"
#include <vector>
#include <memory>

class ASTExpressionNode : ASTNode {
    public:
        ASTExpressionNode(int i);
    private:
        int i;
};

#endif
