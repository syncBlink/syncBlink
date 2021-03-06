#ifndef ARRAYASSIGNSTATEMENT_H
#define ARRAYASSIGNSTATEMENT_H

#include "blinkscript/scanner/model/token.hpp"
#include "blinkscript/parser/ast/ast_node.hpp"
#include "blinkscript/parser/ast/ast_visitor.hpp"
#include "blinkscript/parser/ast/expressions/index_expression.hpp"

namespace SyncBlink
{
    class ArrayAssignStatement : public AstNode
    {
    public:
        ArrayAssignStatement(std::unique_ptr<const AstNode> indexExpression, std::unique_ptr<const AstNode> expression, uint32_t line);
        void accept(AstVisitor& visitor) const;

        uint32_t getLine() const;
        const AstNode& getIndex() const;
        const AstNode* getIndexPtr() const;
        const AstNode& getExpression() const;

    private:
        uint32_t _line;
        std::unique_ptr<const AstNode> _indexExpression;
        std::unique_ptr<const AstNode> _expression;
    };
}

#endif // ARRAYASSIGNSTATEMENT_H