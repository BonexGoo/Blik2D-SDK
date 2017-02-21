#pragma once
#include <addon/blik_fakewin.h>
#include <addon/blik_integration_openssl-1.1.0c.h>

#define LIBSSH2_OPENSSL
#define LIBSSH2_GETTIMEOFDAY_WIN32

#if !BLIK_NDEBUG
    #define LIBSSH2DEBUG
    #define NEED_SSH_DEBUG_LOG 1
#else
    #define NEED_SSH_DEBUG_LOG 0
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// _BLIK_BATCH_COMMAND_ {"type" : "include-alias", "prm" : "BLIK_LIBSSH2_", "restore-comment" : " //original-code:"}
#define BLIK_LIBSSH2_V_libssh2_h                      <addon/libssh2-1.6.0_for_blik/include/libssh2.h>
#define BLIK_LIBSSH2_U_libssh2_h                      "addon/libssh2-1.6.0_for_blik/include/libssh2.h"
#define BLIK_LIBSSH2_U_libssh2_publickey_h            "addon/libssh2-1.6.0_for_blik/include/libssh2_publickey.h"
#define BLIK_LIBSSH2_U_libssh2_sftp_h                 "addon/libssh2-1.6.0_for_blik/include/libssh2_sftp.h"
