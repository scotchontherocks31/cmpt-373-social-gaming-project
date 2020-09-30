#ifndef CHAT_AST_NODEVISITOR_H
#define CHAT_AST_NODEVISITOR_H

#include "Node.h"

class NodeVisitor{
public:
    virtual void visit();
};

#endif