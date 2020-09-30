#ifndef CHAT_AST_NODE_H
#define CHAT_AST_NODE_H

#include <vector>
#include "NodeVisitor.h"

class Node{
private:
    std::vector<std::unique_ptr<Node>> children;
    Node* parent;

public:
    virtual ~Node() = 0;
    
    std::vector<std::unique_ptr<Node>>& getChildren() const;
    Node& getChild(int) const;
    int childrenCount() const;
    virtual void accept(NodeVisitor& nodeVisitor);
    Node& getParent() const;

protected:
    void visitChildren(NodeVisitor& nodeVisitor);
};
#endif