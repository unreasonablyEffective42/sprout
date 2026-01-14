#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "ast_fwd.h"
#include "cell.h"
#include "token.h"
#include "value.h"
#include <iostream>
#include <unordered_map>

struct TypeErr {
  // todo
};

struct TypeEnv {
  std::unordered_map<std::string, class Tp>
}

#endif // !TYPE_CHECKER_H
