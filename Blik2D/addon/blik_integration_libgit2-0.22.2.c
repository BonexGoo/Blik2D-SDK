#include <blik.h>
#if BLIK_NEED_ADDON_GIT

#include <addon/blik_fakewin.h>
#include "blik_integration_libgit2-0.22.2.h"

#if BLIK_WINDOWS
    #pragma comment(lib, "advapi32.lib")
#endif

#if BLIK_WINDOWS
    #define DOT_GIT ".git"
    #define DOT_GIT_MODULE_A ( \
        tolower(path->ptr[len - 4]) != '.' || \
        tolower(path->ptr[len - 3]) != 'g' || \
        tolower(path->ptr[len - 2]) != 'i' || \
        tolower(path->ptr[len - 1]) != 't')
    #define DOT_GIT_MODULE_B ( \
        next_hfs_char(&path, &len) != '.' || \
        next_hfs_char(&path, &len) != 'g' || \
        next_hfs_char(&path, &len) != 'i' || \
        next_hfs_char(&path, &len) != 't' || \
        next_hfs_char(&path, &len) != 0)
    #define DOT_GIT_MODULE_C ( \
        component[0] == '.' && \
        (component[1] == 'g' || component[1] == 'G') && \
        (component[2] == 'i' || component[2] == 'I') && \
        (component[3] == 't' || component[3] == 'T'))
#else
    #define DOT_GIT "_git"
    #define DOT_GIT_MODULE_A ( \
        tolower(path->ptr[len - 4]) != '_' || \
        tolower(path->ptr[len - 3]) != 'g' || \
        tolower(path->ptr[len - 2]) != 'i' || \
        tolower(path->ptr[len - 1]) != 't')
    #define DOT_GIT_MODULE_B ( \
        next_hfs_char(&path, &len) != '_' || \
        next_hfs_char(&path, &len) != 'g' || \
        next_hfs_char(&path, &len) != 'i' || \
        next_hfs_char(&path, &len) != 't' || \
        next_hfs_char(&path, &len) != 0)
    #define DOT_GIT_MODULE_C ( \
        component[0] == '_' && \
        (component[1] == 'g' || component[1] == 'G') && \
        (component[2] == 'i' || component[2] == 'I') && \
        (component[3] == 't' || component[3] == 'T'))
#endif
#define DOT_GIT_LEN (sizeof(DOT_GIT) - 1)

#include BLIK_LIBGIT2_U_git2__attr_h
#include BLIK_LIBGIT2_U_git2__blob_h
#include BLIK_LIBGIT2_U_git2__index_h
#include BLIK_LIBGIT2_U_git2__sys__filter_h
#include BLIK_LIBGIT2_U_common_h
#include <addon/libgit2-0.22.2_for_blik/src/errors.c>
#include <addon/libgit2-0.22.2_for_blik/src/win32/error.c>

#define giterr_set(...) blik_giterr_set(BLIK_DBG __VA_ARGS__)
void blik_giterr_set(BLIK_DBG_PRM int error_class, const char *string, ...)
{
    char Err[1024];
    va_list Args;
    va_start(Args, string);
    blik_vsnprintf(Err, 1024, string, Args);
    va_end(Args);

    BLIK_ASSERT_PRM(Err, false);
}

#define printf(...) blik_printf(BLIK_DBG __VA_ARGS__)
void blik_printf(BLIK_DBG_PRM const char *string, ...)
{
    char Err[1024];
    va_list Args;
    va_start(Args, string);
    blik_vsnprintf(Err, 1024, string, Args);
    va_end(Args);

    BLIK_TRACE("blik_printf(%s) - %s, %d, %s", Err, BLIK_DBG_FILE, BLIK_DBG_LINE, BLIK_DBG_FUNC);
}

#if !BLIK_WINDOWS
    void qsort_s(
        void *els, size_t nel, size_t elsize, git__sort_r_cmp cmp, void *payload)
    {
        BLIK_ASSERT("qsort_s가 준비되지 않았습니다", false);
    }
#endif

#define NO_DUMMY_DECL
#include <addon/libgit2-0.22.2_for_blik/src/strmap.h>
GIT__USE_STRMAP;

#undef GIT__USE_STRMAP
#define GIT__USE_STRMAP
#include <addon/libgit2-0.22.2_for_blik/src/oidmap.h>
GIT__USE_OIDMAP;

#undef GIT__USE_OIDMAP
#define GIT__USE_OIDMAP
#include <addon/libgit2-0.22.2_for_blik/src/annotated_commit.c>
#include <addon/libgit2-0.22.2_for_blik/src/attr.c>
#include <addon/libgit2-0.22.2_for_blik/src/attr_file.c>
#include <addon/libgit2-0.22.2_for_blik/src/attrcache.c>
#include <addon/libgit2-0.22.2_for_blik/src/blame.c>
#include <addon/libgit2-0.22.2_for_blik/src/blame_git.c>
#include <addon/libgit2-0.22.2_for_blik/src/blob.c>
#include <addon/libgit2-0.22.2_for_blik/src/branch.c>
#include <addon/libgit2-0.22.2_for_blik/src/buf_text.c>
#include <addon/libgit2-0.22.2_for_blik/src/buffer.c>
#include <addon/libgit2-0.22.2_for_blik/src/cache.c>
#include <addon/libgit2-0.22.2_for_blik/src/checkout.c>
#include <addon/libgit2-0.22.2_for_blik/src/cherrypick.c>
#include <addon/libgit2-0.22.2_for_blik/src/clone.c>
#include <addon/libgit2-0.22.2_for_blik/src/commit.c>
#include <addon/libgit2-0.22.2_for_blik/src/commit_list.c>
#include <addon/libgit2-0.22.2_for_blik/src/config.c>
#include <addon/libgit2-0.22.2_for_blik/src/config_cache.c>
#include <addon/libgit2-0.22.2_for_blik/src/config_file.c>
#include <addon/libgit2-0.22.2_for_blik/src/crlf.c>
#include <addon/libgit2-0.22.2_for_blik/src/date.c>
#include <addon/libgit2-0.22.2_for_blik/src/delta.c>
#include <addon/libgit2-0.22.2_for_blik/src/delta-apply.c>
#include <addon/libgit2-0.22.2_for_blik/src/describe.c>
#include <addon/libgit2-0.22.2_for_blik/src/diff.c>
#include <addon/libgit2-0.22.2_for_blik/src/diff_driver.c>
#include <addon/libgit2-0.22.2_for_blik/src/diff_file.c>
#include <addon/libgit2-0.22.2_for_blik/src/diff_patch.c>
#include <addon/libgit2-0.22.2_for_blik/src/diff_print.c>
#include <addon/libgit2-0.22.2_for_blik/src/diff_stats.c>
#include <addon/libgit2-0.22.2_for_blik/src/diff_tform.c>
#include <addon/libgit2-0.22.2_for_blik/src/diff_xdiff.c>
#include <addon/libgit2-0.22.2_for_blik/src/fetch.c>
#include <addon/libgit2-0.22.2_for_blik/src/fetchhead.c>
#include <addon/libgit2-0.22.2_for_blik/src/filebuf.c>
#include <addon/libgit2-0.22.2_for_blik/src/fileops.c>
#include <addon/libgit2-0.22.2_for_blik/src/filter.c>
#include <addon/libgit2-0.22.2_for_blik/src/fnmatch.c>
#include <addon/libgit2-0.22.2_for_blik/src/global.c>
#include <addon/libgit2-0.22.2_for_blik/src/graph.c>
#include <addon/libgit2-0.22.2_for_blik/src/hash.c>
#include <addon/libgit2-0.22.2_for_blik/src/hashsig.c>
#include <addon/libgit2-0.22.2_for_blik/src/ident.c>
#include <addon/libgit2-0.22.2_for_blik/src/ignore.c>
#include <addon/libgit2-0.22.2_for_blik/src/index.c>
#include <addon/libgit2-0.22.2_for_blik/src/indexer.c>
#include <addon/libgit2-0.22.2_for_blik/src/iterator.c>
#include <addon/libgit2-0.22.2_for_blik/src/merge.c>
#include <addon/libgit2-0.22.2_for_blik/src/merge_file.c>
#include <addon/libgit2-0.22.2_for_blik/src/message.c>
#include <addon/libgit2-0.22.2_for_blik/src/mwindow.c>
#include <addon/libgit2-0.22.2_for_blik/src/netops.c>
#include <addon/libgit2-0.22.2_for_blik/src/notes.c>
#include <addon/libgit2-0.22.2_for_blik/src/object.c>
#include <addon/libgit2-0.22.2_for_blik/src/object_api.c>
#include <addon/libgit2-0.22.2_for_blik/src/odb.c>
#include <addon/libgit2-0.22.2_for_blik/src/odb_loose.c>
#include <addon/libgit2-0.22.2_for_blik/src/odb_mempack.c>
#include <addon/libgit2-0.22.2_for_blik/src/odb_pack.c>
#include <addon/libgit2-0.22.2_for_blik/src/oid.c>
#include <addon/libgit2-0.22.2_for_blik/src/oidarray.c>
#include <addon/libgit2-0.22.2_for_blik/src/openssl_stream.c>
#include <addon/libgit2-0.22.2_for_blik/src/pack.c>
#include <addon/libgit2-0.22.2_for_blik/src/pack-objects.c>
#include <addon/libgit2-0.22.2_for_blik/src/path.c>
#include <addon/libgit2-0.22.2_for_blik/src/pathspec.c>
#include <addon/libgit2-0.22.2_for_blik/src/pool.c>
#include <addon/libgit2-0.22.2_for_blik/src/posix.c>
#include <addon/libgit2-0.22.2_for_blik/src/pqueue.c>
#include <addon/libgit2-0.22.2_for_blik/src/push.c>
#include <addon/libgit2-0.22.2_for_blik/src/rebase.c>
#include <addon/libgit2-0.22.2_for_blik/src/refdb.c>
#include <addon/libgit2-0.22.2_for_blik/src/refdb_fs.c>
#include <addon/libgit2-0.22.2_for_blik/src/reflog.c>
#include <addon/libgit2-0.22.2_for_blik/src/refs.c>
#include <addon/libgit2-0.22.2_for_blik/src/refspec.c>
#include <addon/libgit2-0.22.2_for_blik/src/remote.c>
#include <addon/libgit2-0.22.2_for_blik/src/repository.c>
#include <addon/libgit2-0.22.2_for_blik/src/reset.c>
#include <addon/libgit2-0.22.2_for_blik/src/revert.c>
#include <addon/libgit2-0.22.2_for_blik/src/revparse.c>
#include <addon/libgit2-0.22.2_for_blik/src/revwalk.c>
#include <addon/libgit2-0.22.2_for_blik/src/settings.c>
#include <addon/libgit2-0.22.2_for_blik/src/sha1_lookup.c>
#include <addon/libgit2-0.22.2_for_blik/src/signature.c>
#include <addon/libgit2-0.22.2_for_blik/src/socket_stream.c>
#include <addon/libgit2-0.22.2_for_blik/src/sortedcache.c>
#include <addon/libgit2-0.22.2_for_blik/src/stash.c>
#include <addon/libgit2-0.22.2_for_blik/src/status.c>
#include <addon/libgit2-0.22.2_for_blik/src/strmap.c>
#include <addon/libgit2-0.22.2_for_blik/src/submodule.c>
#include <addon/libgit2-0.22.2_for_blik/src/sysdir.c>
#include <addon/libgit2-0.22.2_for_blik/src/tag.c>
#include <addon/libgit2-0.22.2_for_blik/src/thread-utils.c>
#include <addon/libgit2-0.22.2_for_blik/src/trace.c>
#include <addon/libgit2-0.22.2_for_blik/src/transaction.c>
#include <addon/libgit2-0.22.2_for_blik/src/transport.c>
#include <addon/libgit2-0.22.2_for_blik/src/tree.c>
#include <addon/libgit2-0.22.2_for_blik/src/tree-cache.c>
#include <addon/libgit2-0.22.2_for_blik/src/tsort.c>
#include <addon/libgit2-0.22.2_for_blik/src/util.c>
#include <addon/libgit2-0.22.2_for_blik/src/vector.c>
#include <addon/libgit2-0.22.2_for_blik/src/zstream.c>

#define BLIK_LIBGIT2_U_buffer_h                       "addon/libgit2-0.22.2_for_blik/src/buffer.h"
#define BLIK_LIBGIT2_U_netops_h                       "addon/libgit2-0.22.2_for_blik/src/netops.h"
#define BLIK_LIBGIT2_U_path_h                         "addon/libgit2-0.22.2_for_blik/src/path.h"
#define BLIK_LIBGIT2_U_stream_h                       "addon/libgit2-0.22.2_for_blik/src/stream.h"
#define BLIK_LIBGIT2_U_socket_stream_h                "addon/libgit2-0.22.2_for_blik/src/socket_stream.h"
#define BLIK_LIBGIT2_U_openssl_stream_h               "addon/libgit2-0.22.2_for_blik/src/openssl_stream.h"
#define BLIK_LIBGIT2_U_global_h                       "addon/libgit2-0.22.2_for_blik/src/global.h"
#define BLIK_LIBGIT2_U_refs_h                         "addon/libgit2-0.22.2_for_blik/src/refs.h"
#define BLIK_LIBGIT2_U_posix_h                        "addon/libgit2-0.22.2_for_blik/src/posix.h"
#define BLIK_LIBGIT2_U_repository_h                   "addon/libgit2-0.22.2_for_blik/src/repository.h"
#define BLIK_LIBGIT2_U_odb_h                          "addon/libgit2-0.22.2_for_blik/src/odb.h"
#define BLIK_LIBGIT2_U_push_h                         "addon/libgit2-0.22.2_for_blik/src/push.h"
#define BLIK_LIBGIT2_U_remote_h                       "addon/libgit2-0.22.2_for_blik/src/remote.h"
#define BLIK_LIBGIT2_U_refspec_h                      "addon/libgit2-0.22.2_for_blik/src/refspec.h"
#define BLIK_LIBGIT2_U_vector_h                       "addon/libgit2-0.22.2_for_blik/src/vector.h"
#define BLIK_LIBGIT2_U_util_h                         "addon/libgit2-0.22.2_for_blik/src/util.h"
#define BLIK_LIBGIT2_U_map_h                          "addon/libgit2-0.22.2_for_blik/src/map.h"
#define BLIK_LIBGIT2_U_pack_M_objects_h               "addon/libgit2-0.22.2_for_blik/src/pack-objects.h"

#include <addon/libgit2-0.22.2_for_blik/src/hash/hash_generic.c>
#include <addon/libgit2-0.22.2_for_blik/src/transports/auth.c>
#include <addon/libgit2-0.22.2_for_blik/src/transports/auth_negotiate.c>
#include <addon/libgit2-0.22.2_for_blik/src/transports/cred.c>
#include <addon/libgit2-0.22.2_for_blik/src/transports/cred_helpers.c>
#include <addon/libgit2-0.22.2_for_blik/src/transports/git.c>
#include <addon/libgit2-0.22.2_for_blik/src/transports/http.c>
#include <addon/libgit2-0.22.2_for_blik/src/transports/local.c>
#include <addon/libgit2-0.22.2_for_blik/src/transports/smart.c>
#include <addon/libgit2-0.22.2_for_blik/src/transports/smart_pkt.c>
#include <addon/libgit2-0.22.2_for_blik/src/transports/smart_protocol.c>
#include <addon/libgit2-0.22.2_for_blik/src/transports/ssh.c>
#include <addon/libgit2-0.22.2_for_blik/src/win32/dir.c>
#include <addon/libgit2-0.22.2_for_blik/src/win32/findfile.c>
#include <addon/libgit2-0.22.2_for_blik/src/win32/map.c>
#include <addon/libgit2-0.22.2_for_blik/src/win32/path_w32.c>
#include <addon/libgit2-0.22.2_for_blik/src/win32/posix_w32.c>
#if BLIK_WINDOWS
    #include <addon/libgit2-0.22.2_for_blik/src/win32/pthread.c>
#endif
#include <addon/libgit2-0.22.2_for_blik/src/win32/utf-conv.c>
#include <addon/libgit2-0.22.2_for_blik/src/win32/w32_util.c>
#include <addon/libgit2-0.22.2_for_blik/src/xdiff/xdiffi.c>
#include <addon/libgit2-0.22.2_for_blik/src/xdiff/xemit.c>
#include <addon/libgit2-0.22.2_for_blik/src/xdiff/xhistogram.c>
#include <addon/libgit2-0.22.2_for_blik/src/xdiff/xmerge.c>
#include <addon/libgit2-0.22.2_for_blik/src/xdiff/xpatience.c>
#include <addon/libgit2-0.22.2_for_blik/src/xdiff/xprepare.c>
#include <addon/libgit2-0.22.2_for_blik/src/xdiff/xutils.c>

#include <addon/libgit2-0.22.2_for_blik/deps/http-parser/http_parser.c>
#include <addon/libgit2-0.22.2_for_blik/deps/regex/regex.c>
#include <addon/libgit2-0.22.2_for_blik/deps/zlib/adler32.c>
#include <addon/libgit2-0.22.2_for_blik/deps/zlib/crc32.c>
#include <addon/libgit2-0.22.2_for_blik/deps/zlib/deflate.c>
#include <addon/libgit2-0.22.2_for_blik/deps/zlib/infback.c>
#include <addon/libgit2-0.22.2_for_blik/deps/zlib/inffast.c>
#include <addon/libgit2-0.22.2_for_blik/deps/zlib/inflate.c>
#include <addon/libgit2-0.22.2_for_blik/deps/zlib/inftrees.c>
#include <addon/libgit2-0.22.2_for_blik/deps/zlib/trees.c>
#include <addon/libgit2-0.22.2_for_blik/deps/zlib/zutil.c>

#endif
