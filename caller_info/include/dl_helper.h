#ifndef _DL_HELPER_
#define _DL_HELPER_

#include <dlfcn.h>
#include <stdio.h>

#include "link.h"
#include "rtld.h"

extern int _dl_addr(const void *address, Dl_info *info, struct link_map **mapp, const ElfW(Sym) * *symbolp);

static struct link_map *dl_find_dso_for_object(const ElfW(Addr) addr) {
    decltype(&dl_find_dso_for_object) _dl_finder = nullptr;
    void *main_handle = dlopen(NULL, RTLD_LOCAL | RTLD_NOW);
    if (!main_handle) {
        fprintf(stderr, "dlopen() failed, error msg: %s\n", dlerror());
    } else {
        _dl_finder = reinterpret_cast<decltype(&dl_find_dso_for_object)>(dlsym(main_handle, "_dl_find_dso_for_object"));
        if (!_dl_finder) {
            fprintf(stderr, "dlsym() failed, error msg: %s\n", dlerror());
        } else {
            printf("Use _dl_find_dso_for_object() provided in libc.so.6\n");
            return _dl_finder(addr);
        }
    }
    
    struct link_map *ret_map = nullptr;
    Dl_info dl_info{};
    auto *_dl_addr_ptr = reinterpret_cast<decltype(&_dl_addr)>(dlsym(main_handle, "_dl_addr"));
    if (!_dl_addr_ptr || _dl_addr_ptr((void *)addr, &dl_info, &ret_map, NULL) == 0) {
        return nullptr;
    }
    return ret_map;
}

#endif  // _DL_HELPER_