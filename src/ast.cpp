#include "ast.h"

std::ostream& operator<<(std::ostream& os, const AstPtr& ast) {
    if (!ast) {
        os << "null-ast";
        return os;
    }
    os << ast->node;
    return os;
}
