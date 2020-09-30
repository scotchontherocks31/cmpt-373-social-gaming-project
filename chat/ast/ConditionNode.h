#ifndef CHAT_AST_CONDITIONNODE_H
#define CHAT_AST_CONDITIONNODE_H

#include "Node.h"

class ConditionNode : public Node {
public:
    bool getCondition();
    //Ard: needs a lambda func
};
#endif CHAT_AST_CONDITIONNODE_H