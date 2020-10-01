#ifndef AST_BLOCK_H
#define AST_BLOCK_H

#include "ASTNode.h"
#include <vector>
#include <memory>

class ASTBlock : ASTNode {
    public:
        ASTBlock(std::vector<std::unique_ptr<ASTNode>> &&children);
    private:
        std::vector<std::unique_ptr<ASTNode>> children;
};

#endif
