#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "token.c"
#include "memory.c"

static void test(bool is_working, const char* msg) {
    if (is_working) {
        printf(" [\x1b[92mOK\x1b[0m]: %s\n", msg);
    } else {
        printf(" [\x1b[91mWrong\x1b[0m]: %s\n", msg);
    }
}

int main(void) {
    const char* end;
    struct Token asdf = isTokenString("\"\\x41\" asdf", &end);
    fprintf(stderr, "%s\n%s\n%d\n", end, 
            asdf.value_string.string,
            asdf.value_string.length
            );
    return 0;
}
