#ifndef INPUT_NODE_VISITOR_H
#define INPUT_NODE_VISITOR_H
#include "ASTNode.h"
#include "ASTNode.h"
#include <iostream>
#include <string>
#include <task.h>
#include <variant>
#include <string>


namespace AST
{
    class InputNodeVisitor : public ASTVisitor //TODO figure out what is wrong with it - class doe not inherit from the parent class
    {
    private:
        std::string message;
        ASTNode **children;

    public:
        void getInput();
        std::string getMessage();
        void getChildren(ASTNode &childrenList);
    };
} // namespace AST
#endif