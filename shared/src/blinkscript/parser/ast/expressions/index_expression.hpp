#ifndef INDEXEXPRESSION_H
#define INDEXEXPRESSION_H

#include "blinkscript/parser/ast/ast_node.hpp"
#include "blinkscript/scanner/model/token.hpp"

namespace SyncBlink
{
    class IndexExpression : public AstNode
    {
    public:
        IndexExpression(std::unique_ptr<const AstNode> left, std::unique_ptr<const AstNode> right, int line);

        void accept(AstVisitor& visitor) const;

        int getLine() const;
        const AstNode& getLeft() const;
        const AstNode& getRight() const;

    private:
        std::unique_ptr<const AstNode> _left;
        std::unique_ptr<const AstNode> _right;
        int _line;
    };
}

#endif // INDEXEXPRESSION_H