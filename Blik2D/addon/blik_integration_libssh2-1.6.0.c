#include <blik.h>
#if BLIK_NEED_ADDON_GIT

#include <addon/blik_fakewin.h>
#include "blik_integration_libssh2-1.6.0.h"

#if BLIK_WINDOWS
    #define HAVE_IOCTLSOCKET
    #if BLIK_NEED_FORCED_FAKEWIN
        #define HAVE_DISABLED_NONBLOCKING
    #else
        #include <ws2tcpip.h>
    #endif
#else
    #define HAVE_DISABLED_NONBLOCKING
#endif

#include BLIK_OPENSSL_V_openssl__rsa_h
#include BLIK_OPENSSL_V_openssl__md5_h
#include <addon/libssh2-1.6.0_for_blik/include/libssh2.h>

#include <addon/libssh2-1.6.0_for_blik/src/openssl.c>
#include <addon/libssh2-1.6.0_for_blik/src/session.c>
#include <addon/libssh2-1.6.0_for_blik/src/hostkey.c>
#include <addon/libssh2-1.6.0_for_blik/src/userauth.c>
#include <addon/libssh2-1.6.0_for_blik/src/channel.c>
#include <addon/libssh2-1.6.0_for_blik/src/agent.c>
#include <addon/libssh2-1.6.0_for_blik/src/transport.c>
#include <addon/libssh2-1.6.0_for_blik/src/packet.c>
#include <addon/libssh2-1.6.0_for_blik/src/global.c>
#include <addon/libssh2-1.6.0_for_blik/src/kex.c>
#include <addon/libssh2-1.6.0_for_blik/src/comp.c>
#include <addon/libssh2-1.6.0_for_blik/src/mac.c>
#include <addon/libssh2-1.6.0_for_blik/src/crypt.c>
#include <addon/libssh2-1.6.0_for_blik/src/misc.c>
#include <addon/libssh2-1.6.0_for_blik/src/keepalive.c>

#endif
