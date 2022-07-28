/* Data structure for communication from the run-time dynamic linker for
   loaded ELF shared objects.
   Copyright (C) 1995-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _LINK_H
#define _LINK_H

#include <bits/wordsize.h>
#include <dlfcn.h>
#include <features.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

#define __ELF_NATIVE_CLASS __WORDSIZE
typedef uint32_t Elf_Symndx;

#include <elf.h>

/* We use this macro to refer to ELF types independent of the native wordsize.
   `ElfW(TYPE)' is used in place of `Elf32_TYPE' or `Elf64_TYPE'.  */
#define ElfW(type) _ElfW(Elf, __ELF_NATIVE_CLASS, type)
#define _ElfW(e, w, t) _ElfW_1(e, w, _##t)
#define _ElfW_1(e, w, t) e##w##t

#define DT_THISPROCNUM 0

/* The type of the return value of fixup/profile_fixup.  */
#define DL_FIXUP_VALUE_TYPE ElfW(Addr)
/* Construct a value of type DL_FIXUP_VALUE_TYPE from a code address
   and a link map.  */
#define DL_FIXUP_MAKE_VALUE(map, addr) (addr)
/* Extract the code address from a value of type DL_FIXUP_MAKE_VALUE.
 */
#define DL_FIXUP_VALUE_CODE_ADDR(value) (value)
#define DL_FIXUP_VALUE_ADDR(value) (value)
#define DL_FIXUP_ADDR_VALUE(addr) (addr)

#define RETURN_ADDRESS(nr) \
    __builtin_extract_return_addr(__builtin_return_address(nr))

/* Some internal data structures of the dynamic linker used in the
   linker map.  */
struct r_found_version {
    const char *name;
    ElfW(Word) hash;

    int hidden;
    const char *filename;
};

enum r_dir_status {
    unknown,
    nonexisting,
    existing
};

struct r_search_path_elem {
    /* This link is only used in the `all_dirs' member of `r_search_path'.  */
    struct r_search_path_elem *next;

    /* Strings saying where the definition came from.  */
    const char *what;
    const char *where;

    /* Basename for this search path element.  The string must end with
       a slash character.  */
    const char *dirname;
    size_t dirnamelen;

    enum r_dir_status status[0];
};

struct libname_list {
    const char *name;          /* Name requested (before search).  */
    struct libname_list *next; /* Link to next name for this object.  */
    int dont_free;             /* Flag whether this element should be freed
				   if the object is not entirely unloaded.  */
};

/* Forward declaration.  */
struct link_map;

/* Structure to describe a single list of scope elements.  The lookup
   functions get passed an array of pointers to such structures.  */
struct r_scope_elem {
    /* Array of maps for the scope.  */
    struct link_map **r_list;
    /* Number of entries in the scope.  */
    unsigned int r_nlist;
};

/* Structure to record search path and allocation mechanism.  */
struct r_search_path_struct {
    struct r_search_path_elem **dirs;
    int malloced;
};

/* For POSIX.1 systems, the pair of st_dev and st_ino constitute
   a unique identifier for a file.  */
struct r_file_id {
    dev_t dev;
    ino64_t ino;
};

static inline bool
_dl_get_file_id(int fd, struct r_file_id *id) {
    struct stat64 st;

    if (__glibc_unlikely(__fxstat64(_STAT_VER, fd, &st) < 0))
        return false;

    id->dev = st.st_dev;
    id->ino = st.st_ino;
    return true;
}

/* Compare two results from _dl_get_file_id for equality.  */
static inline bool
_dl_file_id_match_p(const struct r_file_id *a, const struct r_file_id *b) {
    return a->dev == b->dev && a->ino == b->ino;
}

#if __WORDSIZE == 64
struct link_map_machine {
    Elf64_Addr plt;      /* Address of .plt + 0x16 */
    Elf64_Addr gotplt;   /* Address of .got + 0x18 */
    void *tlsdesc_table; /* Address of TLS descriptor hash table.  */
};

#else
struct link_map_machine {
    Elf32_Addr plt;      /* Address of .plt + 0x16 */
    Elf32_Addr gotplt;   /* Address of .got + 0x0c */
    void *tlsdesc_table; /* Address of TLS descriptor hash table.  */
};
#endif

struct r_debug {
    int r_version; /* Version number for this protocol.  */

    struct link_map *r_map; /* Head of the chain of loaded objects.  */

    /* This is the address of a function internal to the run-time linker,
       that will always be called when the linker begins to map in a
       library or unmap it, and again when the mapping change is complete.
       The debugger can set a breakpoint at this address if it wants to
       notice shared object mapping changes.  */
    ElfW(Addr) r_brk;
    enum {
        /* This state value describes the mapping change taking place when
	   the `r_brk' address is called.  */
        RT_CONSISTENT, /* Mapping change is complete.  */
        RT_ADD,        /* Beginning to add a new object.  */
        RT_DELETE      /* Beginning to remove an object mapping.  */
    } r_state;

    ElfW(Addr) r_ldbase; /* Base address the linker is loaded at.  */
};

/* Structure describing a loaded shared object.  The `l_next' and `l_prev'
   members form a chain of all the shared objects loaded at startup.

   These data structures exist in space used by the run-time dynamic linker;
   modifying them may have disastrous results.

   This data structure might change in future, if necessary.  User-level
   programs must avoid defining objects of this type.  */

struct link_map {
    /* These first few members are part of the protocol with the debugger.
       This is the same format used in SVR4.  */

    ElfW(Addr) l_addr;                /* Difference between the address in the ELF
				   file and the addresses in memory.  */
    char *l_name;                     /* Absolute file name object was found in.  */
    ElfW(Dyn) * l_ld;                 /* Dynamic section of the shared object.  */
    struct link_map *l_next, *l_prev; /* Chain of loaded objects.  */

    /* All following members are internal to the dynamic linker.
       They may change without notice.  */

    /* This is an element which is only ever different from a pointer to
       the very same copy of this type for ld.so when it is used in more
       than one namespace.  */
    struct link_map *l_real;

    /* Number of the namespace this link map belongs to.  */
    Lmid_t l_ns;

    struct libname_list *l_libname;
    /* Indexed pointers to dynamic section.
       [0,DT_NUM) are indexed by the processor-independent tags.
       [DT_NUM,DT_NUM+DT_THISPROCNUM) are indexed by the tag minus DT_LOPROC.
       [DT_NUM+DT_THISPROCNUM,DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM) are
       indexed by DT_VERSIONTAGIDX(tagvalue).
       [DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM,
	DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM+DT_EXTRANUM) are indexed by
       DT_EXTRATAGIDX(tagvalue).
       [DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM+DT_EXTRANUM,
	DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM+DT_EXTRANUM+DT_VALNUM) are
       indexed by DT_VALTAGIDX(tagvalue) and
       [DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM+DT_EXTRANUM+DT_VALNUM,
	DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM+DT_EXTRANUM+DT_VALNUM+DT_ADDRNUM)
       are indexed by DT_ADDRTAGIDX(tagvalue), see <elf.h>.  */

    ElfW(Dyn) * l_info[DT_NUM + DT_THISPROCNUM + DT_VERSIONTAGNUM + DT_EXTRANUM + DT_VALNUM + DT_ADDRNUM];
    const ElfW(Phdr) * l_phdr; /* Pointer to program header table in core.  */
    ElfW(Addr) l_entry;        /* Entry point location.  */
    ElfW(Half) l_phnum;        /* Number of program header entries.  */
    ElfW(Half) l_ldnum;        /* Number of dynamic segment entries.  */

    /* Array of DT_NEEDED dependencies and their dependencies, in
       dependency order for symbol lookup (with and without
       duplicates).  There is no entry before the dependencies have
       been loaded.  */
    struct r_scope_elem l_searchlist;

    /* We need a special searchlist to process objects marked with
       DT_SYMBOLIC.  */
    struct r_scope_elem l_symbolic_searchlist;

    /* Dependent object that first caused this object to be loaded.  */
    struct link_map *l_loader;

    /* Array with version names.  */
    struct r_found_version *l_versions;
    unsigned int l_nversions;

    /* Symbol hash table.  */
    Elf_Symndx l_nbuckets;
    Elf32_Word l_gnu_bitmask_idxbits;
    Elf32_Word l_gnu_shift;
    const ElfW(Addr) * l_gnu_bitmask;
    union {
        const Elf32_Word *l_gnu_buckets;
        const Elf_Symndx *l_chain;
    };
    union {
        const Elf32_Word *l_gnu_chain_zero;
        const Elf_Symndx *l_buckets;
    };

    unsigned int l_direct_opencount; /* Reference count for dlopen/dlclose.  */
    enum                             /* Where this object came from.  */
    {
        lt_executable, /* The main executable program.  */
        lt_library,    /* Library needed by main executable.  */
        lt_loaded      /* Extra run-time loaded shared object.  */
    } l_type : 2;
    unsigned int l_relocated : 1;               /* Nonzero if object's relocations done.  */
    unsigned int l_init_called : 1;             /* Nonzero if DT_INIT function called.  */
    unsigned int l_global : 1;                  /* Nonzero if object in _dl_global_scope.  */
    unsigned int l_reserved : 2;                /* Reserved for internal use.  */
    unsigned int l_phdr_allocated : 1;          /* Nonzero if the data structure pointed
					to by `l_phdr' is allocated.  */
    unsigned int l_soname_added : 1;            /* Nonzero if the SONAME is for sure in
				      the l_libname list.  */
    unsigned int l_faked : 1;                   /* Nonzero if this is a faked descriptor
				   without associated file.  */
    unsigned int l_need_tls_init : 1;           /* Nonzero if GL(dl_init_static_tls)
				       should be called on this link map
				       when relocation finishes.  */
    unsigned int l_auditing : 1;                /* Nonzero if the DSO is used in auditing.  */
    unsigned int l_audit_any_plt : 1;           /* Nonzero if at least one audit module
				       is interested in the PLT interception.*/
    unsigned int l_removed : 1;                 /* Nozero if the object cannot be used anymore
				   since it is removed.  */
    unsigned int l_contiguous : 1;              /* Nonzero if inter-segment holes are
				    mprotected or if no holes are present at
				    all.  */
    unsigned int l_symbolic_in_local_scope : 1; /* Nonzero if l_local_scope
						 during LD_TRACE_PRELINKING=1
						 contains any DT_SYMBOLIC
						 libraries.  */
    unsigned int l_free_initfini : 1;           /* Nonzero if l_initfini can be
				       freed, ie. not allocated with
				       the dummy malloc in ld.so.  */

    /* Collected information about own RPATH directories.  */
    struct r_search_path_struct l_rpath_dirs;

    /* Collected results of relocation while profiling.  */
    struct reloc_result {
        DL_FIXUP_VALUE_TYPE addr;
        struct link_map *bound;
        unsigned int boundndx;
        uint32_t enterexit;
        unsigned int flags;
    } * l_reloc_result;

    /* Pointer to the version information if available.  */
    ElfW(Versym) * l_versyms;

    /* String specifying the path where this object was found.  */
    const char *l_origin;

    /* Start and finish of memory map for this object.  l_map_start
       need not be the same as l_addr.  */
    ElfW(Addr) l_map_start, l_map_end;
    /* End of the executable part of the mapping.  */
    ElfW(Addr) l_text_end;

    /* Default array for 'l_scope'.  */
    struct r_scope_elem *l_scope_mem[4];
    /* Size of array allocated for 'l_scope'.  */
    size_t l_scope_max;
    /* This is an array defining the lookup scope for this link map.
       There are initially at most three different scope lists.  */
    struct r_scope_elem **l_scope;

    /* A similar array, this time only with the local scope.  This is
       used occasionally.  */
    struct r_scope_elem *l_local_scope[2];

    /* This information is kept to check for sure whether a shared
       object is the same as one already loaded.  */
    struct r_file_id l_file_id;

    /* Collected information about own RUNPATH directories.  */
    struct r_search_path_struct l_runpath_dirs;

    /* List of object in order of the init and fini calls.  */
    struct link_map **l_initfini;

    /* List of the dependencies introduced through symbol binding.  */
    struct link_map_reldeps {
        unsigned int act;
        struct link_map *list[];
    } * l_reldeps;
    unsigned int l_reldepsmax;

    /* Nonzero if the DSO is used.  */
    unsigned int l_used;

    /* Various flag words.  */
    ElfW(Word) l_feature_1;
    ElfW(Word) l_flags_1;
    ElfW(Word) l_flags;

    /* Temporarily used in `dl_close'.  */
    int l_idx;

    struct link_map_machine l_mach;

    struct
    {
        const ElfW(Sym) * sym;
        int type_class;
        struct link_map *value;
        const ElfW(Sym) * ret;
    } l_lookup_cache;

    /* Thread-local storage related info.  */

    /* Start of the initialization image.  */
    void *l_tls_initimage;
    /* Size of the initialization image.  */
    size_t l_tls_initimage_size;
    /* Size of the TLS block.  */
    size_t l_tls_blocksize;
    /* Alignment requirement of the TLS block.  */
    size_t l_tls_align;
    /* Offset of first byte module alignment.  */
    size_t l_tls_firstbyte_offset;
#ifndef NO_TLS_OFFSET
#define NO_TLS_OFFSET 0
#endif
#ifndef FORCED_DYNAMIC_TLS_OFFSET
#if NO_TLS_OFFSET == 0
#define FORCED_DYNAMIC_TLS_OFFSET -1
#elif NO_TLS_OFFSET == -1
#define FORCED_DYNAMIC_TLS_OFFSET -2
#else
#error "FORCED_DYNAMIC_TLS_OFFSET is not defined"
#endif
#endif
    /* For objects present at startup time: offset in the static TLS block.  */
    ptrdiff_t l_tls_offset;
    /* Index of the module in the dtv array.  */
    size_t l_tls_modid;

    /* Number of thread_local objects constructed by this DSO.  This is
       atomically accessed and modified and is not always protected by the load
       lock.  See also: CONCURRENCY NOTES in cxa_thread_atexit_impl.c.  */
    size_t l_tls_dtor_count;

    /* Information used to change permission after the relocations are
       done.  */
    ElfW(Addr) l_relro_addr;
    size_t l_relro_size;

    unsigned long long int l_serial;

    /* Audit information.  This array apparent must be the last in the
       structure.  Never add something after it.  */
    struct auditstate {
        uintptr_t cookie;
        unsigned int bindflags;
    } l_audit[0];
};

#if __ELF_NATIVE_CLASS == 32
#define symbind symbind32
#elif __ELF_NATIVE_CLASS == 64
#define symbind symbind64
#else
#error "__ELF_NATIVE_CLASS must be defined"
#endif

#ifdef __USE_GNU

/* Version numbers for la_version handshake interface.  */
#define LAV_CURRENT	1

/* Activity types signaled through la_activity.  */
enum
  {
    LA_ACT_CONSISTENT,		/* Link map consistent again.  */
    LA_ACT_ADD,			/* New object will be added.  */
    LA_ACT_DELETE		/* Objects will be removed.  */
  };

/* Values representing origin of name for dynamic loading.  */
enum
  {
    LA_SER_ORIG = 0x01,		/* Original name.  */
    LA_SER_LIBPATH = 0x02,	/* Directory from LD_LIBRARY_PATH.  */
    LA_SER_RUNPATH = 0x04,	/* Directory from RPATH/RUNPATH.  */
    LA_SER_CONFIG = 0x08,	/* Found through ldconfig.  */
    LA_SER_DEFAULT = 0x40,	/* Default directory.  */
    LA_SER_SECURE = 0x80	/* Unused.  */
  };

/* Values for la_objopen return value.  */
enum
  {
    LA_FLG_BINDTO = 0x01,	/* Audit symbols bound to this object.  */
    LA_FLG_BINDFROM = 0x02	/* Audit symbols bound from this object.  */
  };

/* Values for la_symbind flags parameter.  */
enum
  {
    LA_SYMB_NOPLTENTER = 0x01,	/* la_pltenter will not be called.  */
    LA_SYMB_NOPLTEXIT = 0x02,	/* la_pltexit will not be called.  */
    LA_SYMB_STRUCTCALL = 0x04,	/* Return value is a structure.  */
    LA_SYMB_DLSYM = 0x08,	/* Binding due to dlsym call.  */
    LA_SYMB_ALTVALUE = 0x10	/* Value has been changed by a previous
				   la_symbind call.  */
  };

struct dl_phdr_info
  {
    ElfW(Addr) dlpi_addr;
    const char *dlpi_name;
    const ElfW(Phdr) *dlpi_phdr;
    ElfW(Half) dlpi_phnum;

    /* Note: Following members were introduced after the first
       version of this structure was available.  Check the SIZE
       argument passed to the dl_iterate_phdr callback to determine
       whether or not each later member is available.  */

    /* Incremented when a new object may have been added.  */
    __extension__ unsigned long long int dlpi_adds;
    /* Incremented when an object may have been removed.  */
    __extension__ unsigned long long int dlpi_subs;

    /* If there is a PT_TLS segment, its module ID as used in
       TLS relocations, else zero.  */
    size_t dlpi_tls_modid;

    /* The address of the calling thread's instance of this module's
       PT_TLS segment, if it has one and it has been allocated
       in the calling thread, otherwise a null pointer.  */
    void *dlpi_tls_data;
  };

__BEGIN_DECLS

extern int dl_iterate_phdr (int (*__callback) (struct dl_phdr_info *,
					       size_t, void *),
			    void *__data);


/* Prototypes for the ld.so auditing interfaces.  These are not
   defined anywhere in ld.so but instead have to be provided by the
   auditing DSO.  */
extern unsigned int la_version (unsigned int __version);
extern void la_activity (uintptr_t *__cookie, unsigned int __flag);
extern char *la_objsearch (const char *__name, uintptr_t *__cookie,
			   unsigned int __flag);
extern unsigned int la_objopen (struct link_map *__map, Lmid_t __lmid,
				uintptr_t *__cookie);
extern void la_preinit (uintptr_t *__cookie);
extern uintptr_t la_symbind32 (Elf32_Sym *__sym, unsigned int __ndx,
			       uintptr_t *__refcook, uintptr_t *__defcook,
			       unsigned int *__flags, const char *__symname);
extern uintptr_t la_symbind64 (Elf64_Sym *__sym, unsigned int __ndx,
			       uintptr_t *__refcook, uintptr_t *__defcook,
			       unsigned int *__flags, const char *__symname);
extern unsigned int la_objclose (uintptr_t *__cookie);

__END_DECLS

#endif

#endif // _LINK_H
