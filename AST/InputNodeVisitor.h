#ifndef INPUT_NODE_VISITOR_H
#define INPUT_NODE_VISITOR_H
#include "ASTNode.h"
#include "ASTNode.h"
#include <iostream>
#include <map>
#include <string>
#include <task.h>
#include <variant>
#include <string>

namespace AST
{
    class InputNodeVisitor : public AST::ASTVisitor //TODO figure out what is wrong with it - class doe not inherit from the parent class
    {
    private:
        std::string message;
    public:
        void getInput();
        std::string getMessage();
    };
} // namespace AST
#endif