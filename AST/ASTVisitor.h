#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "ASTLoopNode.h"
#include "ASTForEachNode.h"

template<typename T>
class ASTVisitor {
    public:
        virtual T visit(ASTLoopNode&);
        virtual ~ASTVisitor() = 0;
    protected:
        virtual T visitLeave(ASTLoopNode&);
        virtual T visitEnter(ASTLoopNode&);
        virtual T visitLeave(ASTForEachNode&);
        virtual T visitEnter(ASTForEachNode&);
};

#endif
