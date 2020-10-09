#include "ASTNode.h"
#include "ASTVisitor.h"

namespace AST {
    void GlobalMessage::acceptHelper(ASTVisitor& visitor) {
        visitor.visit(*this);
    }
    void GlobalMessage::acceptForChildrenHelper(ASTVisitor& visitor) {
        for (auto& child : children) {
            child->accept(visitor);
        }
    }

    void FormatNode::acceptHelper(ASTVisitor& visitor) {
        visitor.visit(*this);
    }
    void FormatNode::acceptForChildrenHelper(ASTVisitor& visitor) {
        for (auto& child : children) {
            child->accept(visitor);
        }
    }

    
}
