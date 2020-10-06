#ifndef AST_GLOBAL_OUT_NODE_H
#define AST_GLOBAL_OUT_NODE_H

#include "ASTNode.h"
#include <string>

using namespace AST;

class ASTGlobalOutNode : public ASTNode {
    public:
        ASTGlobalOutNode(const std::string&);
        const std::string& getMessage() const;
    private:
        std::string message;
};

#endif
