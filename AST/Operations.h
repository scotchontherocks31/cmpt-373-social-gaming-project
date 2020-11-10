#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "ASTVisitor.h"

namespace AST{
    void dotOperation(Variable left, Variable right); //this will be called initially for every left.right

    template<typename T, typename R>
    void dotOperation(T left, R right);

    template<typename T, typename R>
    void dotOperation(std::vector<T> left, R right);
}
#endif