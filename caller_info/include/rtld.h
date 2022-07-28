#ifndef _RTLD_H_
#define _RTLD_H_

#include <pthread.h>
#include <stdint.h>

#include "link.h"

typedef struct {
    pthread_mutex_t mutex;
} __rtld_lock_recursive_t;

struct link_namespaces {
    /* A pointer to the map for the main map.  */
    struct link_map *_ns_loaded;
    /* Number of object in the _dl_loaded list.  */
    unsigned int _ns_nloaded;
    /* Direct pointer to the searchlist of the main object.  */
    struct r_scope_elem *_ns_main_searchlist;
    /* This is zero at program start to signal that the global scope map is
       allocated by rtld.  Later it keeps the size of the map.  It might be
       reset if in _dl_close if the last global object is removed.  */
    size_t _ns_global_scope_alloc;
    /* Search table for unique objects.  */
    struct unique_sym_table {
        __rtld_lock_recursive_t lock;
        struct unique_sym {
            uint32_t hashval;
            const char *name;
            const ElfW(Sym) * sym;
            const struct link_map *map;
        } * entries;
        size_t size;
        size_t n_elements;
        void (*free)(void *);
    } _ns_unique_sym_table;
    /* Keep track of changes to each namespace' list.  */
    struct r_debug _ns_debug;
};

struct rtld_global {
    /* Don't change the order of the following elements.  'dl_loaded'
     must remain the first element.  Forever.  */
#define DL_NNS 16

    struct link_namespaces _dl_ns[DL_NNS];

    /* One higher than index of last used namespace.  */
    size_t _dl_nns;

    /* During the program run we must not modify the global data of
         loaded shared object simultanously in two threads.  Therefore we
         protect `_dl_open' and `_dl_close' in dl-close.c.

         This must be a recursive lock since the initializer function of
         the loaded object might as well require a call to this function.
         At this time it is not anymore a problem to modify the tables.  */
    __rtld_lock_recursive_t _dl_load_lock;
    /* This lock is used to keep __dl_iterate_phdr from inspecting the
         list of loaded objects while an object is added to or removed
         from that list.  */
    __rtld_lock_recursive_t _dl_load_write_lock;

    /* Incremented whenever something may have been added to dl_loaded.  */
    unsigned long long _dl_load_adds;

    /* The object to be initialized first.  */
    struct link_map *_dl_initfirst;

    /* We have no use for the rest internal parts. */

    // #if HP_SMALL_TIMING_AVAIL
    //     /* Start time on CPU clock.  */
    //     hp_timing_t _dl_cpuclock_offset;
    // #endif

    //     /* Map of shared object to be profiled.  */
    //     struct link_map *_dl_profile_map;

    //     /* Counters for the number of relocations performed.  */
    //     unsigned long int _dl_num_relocations;
    //     unsigned long int _dl_num_cache_relocations;

    //     /* List of search directories.  */
    //     struct r_search_path_elem *_dl_all_dirs;

    //     /* Structure describing the dynamic linker itself.  We need to
    //      reserve memory for the data the audit libraries need.  */
    //     struct link_map _dl_rtld_map;
    //     struct auditstate audit_data[DL_NNS];

    // #if defined SHARED && defined _LIBC_REENTRANT && defined __rtld_lock_default_lock_recursive
    //     void (*_dl_rtld_lock_recursive)(void *);
    //     void (*_dl_rtld_unlock_recursive)(void *);
    // #endif

    //     /* Get architecture specific definitions.  */
    // #define PROCINFO_DECL
    // #ifndef PROCINFO_CLASS
    // #define PROCINFO_CLASS
    // #endif
    // #include <dl-procruntime.c>

    //     /* If loading a shared object requires that we make the stack executable
    //      when it was not, we do it by calling this function.
    //      It returns an errno code or zero on success.  */
    //     int (*_dl_make_stack_executable_hook)(void **);

    //     /* Prevailing state of the stack, PF_X indicating it's executable.  */
    //     ElfW(Word) _dl_stack_flags;

    //     /* Flag signalling whether there are gaps in the module ID allocation.  */
    //     bool _dl_tls_dtv_gaps;
    //     /* Highest dtv index currently needed.  */
    //     size_t _dl_tls_max_dtv_idx;
    //     /* Information about the dtv slots.  */
    //     struct dtv_slotinfo_list {
    //         size_t len;
    //         struct dtv_slotinfo_list *next;
    //         struct dtv_slotinfo {
    //             size_t gen;
    //             struct link_map *map;
    //         } slotinfo[0];
    //     } * _dl_tls_dtv_slotinfo_list;
    //     /* Number of modules in the static TLS block.  */
    //     size_t _dl_tls_static_nelem;
    //     /* Size of the static TLS block.  */
    //     size_t _dl_tls_static_size;
    //     /* Size actually allocated in the static TLS block.  */
    //     size_t _dl_tls_static_used;
    //     /* Alignment requirement of the static TLS block.  */
    //     size_t _dl_tls_static_align;

    // /* Number of additional entries in the slotinfo array of each slotinfo
    //    list element.  A large number makes it almost certain take we never
    //    have to iterate beyond the first element in the slotinfo list.  */
    // #define TLS_SLOTINFO_SURPLUS (62)

    // /* Number of additional slots in the dtv allocated.  */
    // #define DTV_SURPLUS (14)

    //     /* Initial dtv of the main thread, not allocated with normal malloc.  */
    //     void *_dl_initial_dtv;
    //     /* Generation counter for the dtv.  */
    //     size_t _dl_tls_generation;

    //     void (*_dl_init_static_tls)(struct link_map *);

    //     void (*_dl_wait_lookup_done)(void);

    //     /* Scopes to free after next THREAD_GSCOPE_WAIT ().  */
    //     struct dl_scope_free_list {
    //         size_t count;
    //         void *list[50];
    //     } * _dl_scope_free_list;
    // #if !THREAD_GSCOPE_IN_TCB
    //     int _dl_thread_gscope_count;
    // #endif
};

extern struct rtld_global _rtld_global;

#endif  // _RTLD_H_