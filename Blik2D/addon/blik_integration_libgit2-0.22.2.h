#pragma once
#include <addon/blik_fakewin.h>
#include <addon/blik_integration_libssh2-1.6.0.h>

#define NO_VIZ
#define STDC
#define NO_GZIP
#define _FILE_OFFSET_BITS 64
#define GIT_ARCH_32
#define git2_EXPORTS
#define GIT_SSH
#define GIT_STDLIB_CALL
#define STATIC_IMAXDIV
#define INDEX_DEBUG_LOOKUP

////////////////////////////////////////////////////////////////////////////////////////////////////
// _BLIK_BATCH_COMMAND_ {"type" : "include-alias", "prm" : "BLIK_LIBGIT2_", "restore-comment" : " //original-code:"}
#define BLIK_LIBGIT2_V_regex_h                        <addon/libgit2-0.22.2_for_blik/deps/regex/regex.h>
#define BLIK_LIBGIT2_V_zlib_h                         <addon/libgit2-0.22.2_for_blik/deps/zlib/zlib.h>
#define BLIK_LIBGIT2_V_git2_h                         <addon/libgit2-0.22.2_for_blik/include/git2.h>
#define BLIK_LIBGIT2_V_git2__pathspec_h               <addon/libgit2-0.22.2_for_blik/include/git2/pathspec.h>
#define BLIK_LIBGIT2_V_git2__types_h                  <addon/libgit2-0.22.2_for_blik/include/git2/types.h>
#define BLIK_LIBGIT2_V_git2__annotated_commit_h       <addon/libgit2-0.22.2_for_blik/include/git2/annotated_commit.h>
#define BLIK_LIBGIT2_V_git2__rebase_h                 <addon/libgit2-0.22.2_for_blik/include/git2/rebase.h>
#define BLIK_LIBGIT2_V_git2__commit_h                 <addon/libgit2-0.22.2_for_blik/include/git2/commit.h>
#define BLIK_LIBGIT2_V_git2__reset_h                  <addon/libgit2-0.22.2_for_blik/include/git2/reset.h>
#define BLIK_LIBGIT2_V_git2__revwalk_h                <addon/libgit2-0.22.2_for_blik/include/git2/revwalk.h>
#define BLIK_LIBGIT2_V_git2__trace_h                  <addon/libgit2-0.22.2_for_blik/include/git2/trace.h>
#define BLIK_LIBGIT2_V_git2__notes_h                  <addon/libgit2-0.22.2_for_blik/include/git2/notes.h>
#define BLIK_LIBGIT2_V_git2__tag_h                    <addon/libgit2-0.22.2_for_blik/include/git2/tag.h>
#define BLIK_LIBGIT2_V_git2__object_h                 <addon/libgit2-0.22.2_for_blik/include/git2/object.h>
#define BLIK_LIBGIT2_V_git2__refdb_h                  <addon/libgit2-0.22.2_for_blik/include/git2/refdb.h>
#define BLIK_LIBGIT2_V_git2__branch_h                 <addon/libgit2-0.22.2_for_blik/include/git2/branch.h>
#define BLIK_LIBGIT2_V_git2__oid_h                    <addon/libgit2-0.22.2_for_blik/include/git2/oid.h>
#define BLIK_LIBGIT2_V_git2__refs_h                   <addon/libgit2-0.22.2_for_blik/include/git2/refs.h>
#define BLIK_LIBGIT2_V_git2__signature_h              <addon/libgit2-0.22.2_for_blik/include/git2/signature.h>
#define BLIK_LIBGIT2_V_git2__sys__refdb_backend_h     <addon/libgit2-0.22.2_for_blik/include/git2/sys/refdb_backend.h>
#define BLIK_LIBGIT2_V_git2__sys__refs_h              <addon/libgit2-0.22.2_for_blik/include/git2/sys/refs.h>
#define BLIK_LIBGIT2_V_git2__sys__reflog_h            <addon/libgit2-0.22.2_for_blik/include/git2/sys/reflog.h>
#define BLIK_LIBGIT2_U_buffer_h                       "buffer.h"
#define BLIK_LIBGIT2_U_netops_h                       "netops.h"
#define BLIK_LIBGIT2_U_path_h                         "path.h"
#define BLIK_LIBGIT2_U_stream_h                       "stream.h"
#define BLIK_LIBGIT2_U_socket_stream_h                "socket_stream.h"
#define BLIK_LIBGIT2_U_openssl_stream_h               "openssl_stream.h"
#define BLIK_LIBGIT2_U_global_h                       "global.h"
#define BLIK_LIBGIT2_U_refs_h                         "refs.h"
#define BLIK_LIBGIT2_U_posix_h                        "posix.h"
#define BLIK_LIBGIT2_U_repository_h                   "repository.h"
#define BLIK_LIBGIT2_U_odb_h                          "odb.h"
#define BLIK_LIBGIT2_U_push_h                         "push.h"
#define BLIK_LIBGIT2_U_remote_h                       "remote.h"
#define BLIK_LIBGIT2_U_refspec_h                      "refspec.h"
#define BLIK_LIBGIT2_U_vector_h                       "vector.h"
#define BLIK_LIBGIT2_U_util_h                         "util.h"
#define BLIK_LIBGIT2_U_map_h                          "map.h"
#define BLIK_LIBGIT2_U_pack_M_objects_h               "pack-objects.h"
#define BLIK_LIBGIT2_U_common_h                       "addon/libgit2-0.22.2_for_blik/src/common.h"
#define BLIK_LIBGIT2_U_hash_h                         "addon/libgit2-0.22.2_for_blik/src/hash.h"
#define BLIK_LIBGIT2_U_hash__hash_generic_h           "addon/libgit2-0.22.2_for_blik/src/hash/hash_generic.h"
#define BLIK_LIBGIT2_U_git2__cred_helpers_h           "addon/libgit2-0.22.2_for_blik/include/git2/cred_helpers.h"
#define BLIK_LIBGIT2_U_git2__push_h                   "addon/libgit2-0.22.2_for_blik/include/git2/push.h"
#define BLIK_LIBGIT2_U_git2__types_h                  "addon/libgit2-0.22.2_for_blik/include/git2/types.h"
#define BLIK_LIBGIT2_U_git2__errors_h                 "addon/libgit2-0.22.2_for_blik/include/git2/errors.h"
#define BLIK_LIBGIT2_U_git2__common_h                 "addon/libgit2-0.22.2_for_blik/include/git2/common.h"
#define BLIK_LIBGIT2_U_git2__commit_h                 "addon/libgit2-0.22.2_for_blik/include/git2/commit.h"
#define BLIK_LIBGIT2_U_git2__refs_h                   "addon/libgit2-0.22.2_for_blik/include/git2/refs.h"
#define BLIK_LIBGIT2_U_git2__repository_h             "addon/libgit2-0.22.2_for_blik/include/git2/repository.h"
#define BLIK_LIBGIT2_U_git2__annotated_commit_h       "addon/libgit2-0.22.2_for_blik/include/git2/annotated_commit.h"
#define BLIK_LIBGIT2_U_git2__oid_h                    "addon/libgit2-0.22.2_for_blik/include/git2/oid.h"
#define BLIK_LIBGIT2_U_git2__attr_h                   "addon/libgit2-0.22.2_for_blik/include/git2/attr.h"
#define BLIK_LIBGIT2_U_git2__odb_h                    "addon/libgit2-0.22.2_for_blik/include/git2/odb.h"
#define BLIK_LIBGIT2_U_git2__strarray_h               "addon/libgit2-0.22.2_for_blik/include/git2/strarray.h"
#define BLIK_LIBGIT2_U_git2__buffer_h                 "addon/libgit2-0.22.2_for_blik/include/git2/buffer.h"
#define BLIK_LIBGIT2_U_git2_h                         "addon/libgit2-0.22.2_for_blik/include/git2.h"
#define BLIK_LIBGIT2_U_git2__config_h                 "addon/libgit2-0.22.2_for_blik/include/git2/config.h"
#define BLIK_LIBGIT2_U_git2__refdb_h                  "addon/libgit2-0.22.2_for_blik/include/git2/refdb.h"
#define BLIK_LIBGIT2_U_git2__object_h                 "addon/libgit2-0.22.2_for_blik/include/git2/object.h"
#define BLIK_LIBGIT2_U_git2__submodule_h              "addon/libgit2-0.22.2_for_blik/include/git2/submodule.h"
#define BLIK_LIBGIT2_U_git2__blob_h                   "addon/libgit2-0.22.2_for_blik/include/git2/blob.h"
#define BLIK_LIBGIT2_U_git2__tree_h                   "addon/libgit2-0.22.2_for_blik/include/git2/tree.h"
#define BLIK_LIBGIT2_U_git2__revparse_h               "addon/libgit2-0.22.2_for_blik/include/git2/revparse.h"
#define BLIK_LIBGIT2_U_git2__transaction_h            "addon/libgit2-0.22.2_for_blik/include/git2/transaction.h"
#define BLIK_LIBGIT2_U_git2__trace_h                  "addon/libgit2-0.22.2_for_blik/include/git2/trace.h"
#define BLIK_LIBGIT2_U_git2__revwalk_h                "addon/libgit2-0.22.2_for_blik/include/git2/revwalk.h"
#define BLIK_LIBGIT2_U_git2__diff_h                   "addon/libgit2-0.22.2_for_blik/include/git2/diff.h"
#define BLIK_LIBGIT2_U_git2__signature_h              "addon/libgit2-0.22.2_for_blik/include/git2/signature.h"
#define BLIK_LIBGIT2_U_git2__blame_h                  "addon/libgit2-0.22.2_for_blik/include/git2/blame.h"
#define BLIK_LIBGIT2_U_git2__odb_backend_h            "addon/libgit2-0.22.2_for_blik/include/git2/odb_backend.h"
#define BLIK_LIBGIT2_U_git2__status_h                 "addon/libgit2-0.22.2_for_blik/include/git2/status.h"
#define BLIK_LIBGIT2_U_git2__filter_h                 "addon/libgit2-0.22.2_for_blik/include/git2/filter.h"
#define BLIK_LIBGIT2_U_git2__index_h                  "addon/libgit2-0.22.2_for_blik/include/git2/index.h"
#define BLIK_LIBGIT2_U_git2__branch_h                 "addon/libgit2-0.22.2_for_blik/include/git2/branch.h"
#define BLIK_LIBGIT2_U_git2__checkout_h               "addon/libgit2-0.22.2_for_blik/include/git2/checkout.h"
#define BLIK_LIBGIT2_U_git2__merge_h                  "addon/libgit2-0.22.2_for_blik/include/git2/merge.h"
#define BLIK_LIBGIT2_U_git2__clone_h                  "addon/libgit2-0.22.2_for_blik/include/git2/clone.h"
#define BLIK_LIBGIT2_U_git2__remote_h                 "addon/libgit2-0.22.2_for_blik/include/git2/remote.h"
#define BLIK_LIBGIT2_U_git2__cherrypick_h             "addon/libgit2-0.22.2_for_blik/include/git2/cherrypick.h"
#define BLIK_LIBGIT2_U_git2__message_h                "addon/libgit2-0.22.2_for_blik/include/git2/message.h"
#define BLIK_LIBGIT2_U_git2__tag_h                    "addon/libgit2-0.22.2_for_blik/include/git2/tag.h"
#define BLIK_LIBGIT2_U_git2__graph_h                  "addon/libgit2-0.22.2_for_blik/include/git2/graph.h"
#define BLIK_LIBGIT2_U_git2__ignore_h                 "addon/libgit2-0.22.2_for_blik/include/git2/ignore.h"
#define BLIK_LIBGIT2_U_git2__sys__repository_h        "addon/libgit2-0.22.2_for_blik/include/git2/repository.h"
#define BLIK_LIBGIT2_U_git2__revert_h                 "addon/libgit2-0.22.2_for_blik/include/git2/revert.h"
#define BLIK_LIBGIT2_U_git2__indexer_h                "addon/libgit2-0.22.2_for_blik/include/git2/indexer.h"
#define BLIK_LIBGIT2_U_git2__reset_h                  "addon/libgit2-0.22.2_for_blik/include/git2/reset.h"
#define BLIK_LIBGIT2_U_git2__oidarray_h               "addon/libgit2-0.22.2_for_blik/include/git2/oidarray.h"
#define BLIK_LIBGIT2_U_git2__net_h                    "addon/libgit2-0.22.2_for_blik/include/git2/net.h"
#define BLIK_LIBGIT2_U_git2__reflog_h                 "addon/libgit2-0.22.2_for_blik/include/git2/reflog.h"
#define BLIK_LIBGIT2_U_git2__refspec_h                "addon/libgit2-0.22.2_for_blik/include/git2/refspec.h"
#define BLIK_LIBGIT2_U_git2__transport_h              "addon/libgit2-0.22.2_for_blik/include/git2/transport.h"
#define BLIK_LIBGIT2_U_git2__describe_h               "addon/libgit2-0.22.2_for_blik/include/git2/describe.h"
#define BLIK_LIBGIT2_U_git2__patch_h                  "addon/libgit2-0.22.2_for_blik/include/git2/patch.h"
#define BLIK_LIBGIT2_U_git2__pack_h                   "addon/libgit2-0.22.2_for_blik/include/git2/pack.h"
#define BLIK_LIBGIT2_U_git2__stash_h                  "addon/libgit2-0.22.2_for_blik/include/git2/stash.h"
#define BLIK_LIBGIT2_U_git2__global_h                 "addon/libgit2-0.22.2_for_blik/include/git2/global.h"
#define BLIK_LIBGIT2_U_git2__pathspec_h               "addon/libgit2-0.22.2_for_blik/include/git2/pathspec.h"
#define BLIK_LIBGIT2_U_git2__sys__transport_h         "addon/libgit2-0.22.2_for_blik/include/git2/sys/transport.h"
#define BLIK_LIBGIT2_U_git2__sys__diff_h              "addon/libgit2-0.22.2_for_blik/include/git2/sys/diff.h"
#define BLIK_LIBGIT2_U_git2__sys__index_h             "addon/libgit2-0.22.2_for_blik/include/git2/sys/index.h"
#define BLIK_LIBGIT2_U_git2__sys__commit_h            "addon/libgit2-0.22.2_for_blik/include/git2/sys/commit.h"
#define BLIK_LIBGIT2_U_git2__sys__refs_h              "addon/libgit2-0.22.2_for_blik/include/git2/sys/refs.h"
#define BLIK_LIBGIT2_U_git2__sys__config_h            "addon/libgit2-0.22.2_for_blik/include/git2/sys/config.h"
#define BLIK_LIBGIT2_U_git2__sys__filter_h            "addon/libgit2-0.22.2_for_blik/include/git2/sys/filter.h"
#define BLIK_LIBGIT2_U_git2__sys__hashsig_h           "addon/libgit2-0.22.2_for_blik/include/git2/sys/hashsig.h"
#define BLIK_LIBGIT2_U_git2__sys__openssl_h           "addon/libgit2-0.22.2_for_blik/include/git2/sys/openssl.h"
#define BLIK_LIBGIT2_U_git2__sys__stream_h            "addon/libgit2-0.22.2_for_blik/include/git2/sys/stream.h"
#define BLIK_LIBGIT2_U_git2__sys__odb_backend_h       "addon/libgit2-0.22.2_for_blik/include/git2/sys/odb_backend.h"
#define BLIK_LIBGIT2_U_git2__sys__refdb_backend_h     "addon/libgit2-0.22.2_for_blik/include/git2/sys/refdb_backend.h"
#define BLIK_LIBGIT2_U_http_parser_h                  "addon/libgit2-0.22.2_for_blik/deps/http-parser/http_parser.h"
#define BLIK_LIBGIT2_U_win32__pthread_h               "addon/libgit2-0.22.2_for_blik/src/win32/pthread.h"

#if BLIK_WINDOWS && !BLIK_NEED_FORCED_FAKEWIN
    #define BLIK_LIBGIT2_U_win32__msvc_M_compat_h         "addon/libgit2-0.22.2_for_blik/src/win32/msvc-compat.h"
    #define BLIK_LIBGIT2_U_win32__mingw_M_compat_h        "addon/libgit2-0.22.2_for_blik/src/win32/mingw-compat.h"
#else
    #define BLIK_LIBGIT2_U_win32__msvc_M_compat_h         "addon/blik_fakewin.h"
    #define BLIK_LIBGIT2_U_win32__mingw_M_compat_h        "addon/blik_fakewin.h"
#endif
