#include "value.h"
#include "token.h"
#include "cell.h"
#include "fraction.h"

int main() {
    Token tok1 = Token(TokenKind::UNQUOTESPLICE);
    Token tok2 = Token(TokenKind::STRING,"test");
    std::cout << tok1 << std::endl << tok2;
}
