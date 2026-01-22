#include <memory>
#include <ostream>

// forward declaration
struct AstNode;
using AstPtr = std::shared_ptr<const AstNode>;

std::ostream &operator<<(std::ostream &os, const AstPtr &ast);
