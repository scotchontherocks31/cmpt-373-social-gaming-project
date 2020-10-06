#ifndef AST_NODE_H
#define AST_NODE_H

#include <vector>
#include <memory>
#include <algorithm>
#include <string>

namespace AST {

class ASTVisitor;

class ASTNode {
    public:
        int getChildrenCount() const {
            return this->numChildren;
        }
        const std::vector<ASTNode const*> getChildren() const {
            std::vector<ASTNode const*> returnValue;
            for (auto& x : children) {
                returnValue.push_back(x.get());
            }
            return returnValue;
        }
        const ASTNode& getParent() const {
            return *parent;
        }
        void setParent(ASTNode* parent) {
            parent = parent;
        }
        template <typename T>
        void appendChild(T&& child) {
            children.push_back(std::forward<T>(child));
        }
        void accept(ASTVisitor& visitor) {
            acceptHelper(visitor);
        }
        void acceptForChildren(ASTVisitor& visitor) {
            acceptForChildrenHelper(visitor);
        }
        virtual ~ASTNode() {};
    protected:
        std::vector<std::unique_ptr<ASTNode>> children;
        ASTNode* parent;
        int numChildren;
    private:
        virtual void acceptHelper(ASTVisitor& visitor) = 0;
        virtual void acceptForChildrenHelper(ASTVisitor& visitor) = 0;

};

class FormatNode : public ASTNode {
    public:
        FormatNode(std::string &&format) : format{std::move(format)} {}
        const std::string& getFormat() {
            return format;
        }
    private:
        virtual void acceptHelper(ASTVisitor& visitor) override {}
        virtual void acceptForChildrenHelper(ASTVisitor& visitor) override {}
        std::string format;
};

class GlobalMessage : public ASTNode {
    public:
        const FormatNode& getFormateNode() const {
            return *static_cast<FormatNode*>(children[0].get());
        }
    private:
        virtual void acceptHelper(ASTVisitor& visitor) override;
        virtual void acceptForChildrenHelper(ASTVisitor& visitor) override;
};


}

#endif
