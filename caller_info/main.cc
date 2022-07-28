#include <stdio.h>
#include "include/caller.h"

int main(){
    printf("main func calling >> caller!\n");
    caller();
    printf("main func calling << callee!\n");
    callee();

    return 0;
}