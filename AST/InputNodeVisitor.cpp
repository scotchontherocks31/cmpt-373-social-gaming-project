#include "InputNodeVisitor.h"
#include <iostream>
#include <map>
#include <string>
#include <task.h>
#include <variant>
#include <string>

void AST::InputNodeVisitor::getInput()
{
    std::cout << "Your move: ";
    std::cin >> this->message;
    std::cout << std::endl;
}

std::string AST::InputNodeVisitor::getMessage() 
{
    return this->message;
}