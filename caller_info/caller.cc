#include "include/caller.h"

#define RETURN_ADDRESS(nr) \
    __builtin_extract_return_addr(__builtin_return_address(nr))


void callee(){
    printf("<< callee is called!\n");
    void *caller_addr = RETURN_ADDRESS(0);
    struct link_map *l_caller = dl_find_dso_for_object((uint64_t)caller_addr);
    if (!l_caller) {
        printf("Failed to locate suspect %p\n", caller_addr);
    } else {
        printf("Suspect: %p in module %s\n",
                    caller_addr, *l_caller->l_name != '\0' ? l_caller->l_name : "executable");
    }
    printf("<< callee is done!\n");
}


void caller(){
    printf(">> caller is called!\n");
    void *caller_addr = RETURN_ADDRESS(0);
    struct link_map *l_caller = dl_find_dso_for_object((uint64_t)caller_addr);
    if (!l_caller) {
        printf("Failed to locate suspect %p\n", caller_addr);
    } else {
        printf("Suspect: %p in module %s\n",
                    caller_addr, *l_caller->l_name != '\0' ? l_caller->l_name : "executable");
    }
    printf(">> caller is calling callee\n");
    callee();
    printf(">> caller is done calling callee\n");
    printf(">> caller is done!\n");
}

// int main(){
//     printf("main func calling >> caller!\n");
//     caller();
//     printf("main func calling << callee!\n");
//     callee();

//     return 0;
// }
