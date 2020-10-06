#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include <string>
#include <unordered_map>
#include "ASTNode.h"
#include "ASTGlobalOutNode.h"
namespace AST {


// class Communication {
//     public:
//         void sendGlobalMessage(std::string&);
// };

class DSLValue{
    // create enums to check for type first
    public:
        void iamDSL();
        void setString(std::string dslString);
        std::string getString();
    private:
        std::string dslString;
};

class Environment {
    public:

        //Environment& Environment::operator=(Environment other);
        void iam();

        // use const?
        DSLValue& getValue(ASTNode&) ;
    //     void removeBinding(ASTNode&);
    //     bool constains(ASTNode&);
         void setBinding(ASTNode&, DSLValue);
    //     Environment& createChildEnvironment();
    //     void removeChildEnvironment();
    private:
        Environment *parent;
        std::unique_ptr<Environment> child;
       
        std::unordered_map<ASTNode*, DSLValue> bindings;

      
};

class ASTVisitor {
     public:
           
//         virtual void visitLeave(ForEachNode&);
//         virtual void visitEnter(ForEachNode&);
         //virtual void visitLeave(ASTGlobalOutNode&);
           virtual void visitEnter(ASTGlobalOutNode&);
//         virtual void visitLeave(ListNode&);
//         virtual void visitEnter(ListNode&);
//         virtual void visitLeave(ElementNode&);
//         virtual void visitEnter(ElementNode&);
//         virtual void visitLeave(RulesNode&);
//         virtual void visitEnter(RulesNode&);
//         virtual void visitLeave(FormatNode&);
//         virtual void visitEnter(FormatNode&);
//         virtual ~ASTVisitor() = 0;
//     private:
 };

class Interpreter : public ASTVisitor {
    public:
       
        Interpreter(Environment &enviro );
        //void setEnvironment(Environment&);
//         virtual void visitLeave(ForEachNode&) override;
//         virtual void visitEnter(ForEachNode& node) override;
        //virtual void visitLeave(ASTGlobalOutNode&) override;
           void visitEnter(ASTGlobalOutNode&) override;
//         virtual void visitLeave(ListNode&) override;
//         virtual void visitEnter(ListNode&) override;
//         virtual void visitLeave(ElementNode&) override;
//         virtual void visitEnter(ElementNode&) override;
//         virtual void visitLeave(RulesNode&) override;
//         virtual void visitEnter(RulesNode&) override;
//         virtual void visitLeave(FormatNode&) override;
//         virtual void visitEnter(FormatNode&) override;
        
    private:
           Environment &environment; 
//         Communication &communication;
};







}
#endif
