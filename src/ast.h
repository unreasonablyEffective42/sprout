#ifndef SPROUT_LANG_AST_H
#define SPROUT_LANG_AST_H

#include "token.h"
#include <ostream>

// ptr wrapper to avoid circular dependency with allowing tokens to have a value
// of a tokennode
struct AstNode {
    TokenNode node;
    explicit AstNode(TokenNode n) : node(std::move(n)) {}
};

std::ostream &operator<<(std::ostream &os, const AstPtr &ast);

#endif
