/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2016, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include BLIK_CURL_U_curl_setup_h //original-code:"curl_setup.h"

#ifndef CURL_DISABLE_DICT

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include BLIK_CURL_U_urldata_h //original-code:"urldata.h"
#include BLIK_CURL_V_curl__curl_h //original-code:<curl/curl.h>
#include BLIK_CURL_U_transfer_h //original-code:"transfer.h"
#include BLIK_CURL_U_sendf_h //original-code:"sendf.h"
#include BLIK_CURL_U_escape_h //original-code:"escape.h"
#include BLIK_CURL_U_progress_h //original-code:"progress.h"
#include BLIK_CURL_U_dict_h //original-code:"dict.h"
#include BLIK_CURL_U_strcase_h //original-code:"strcase.h"
#include BLIK_CURL_U_curl_memory_h //original-code:"curl_memory.h"
/* The last #include file should be: */
#include BLIK_CURL_U_memdebug_h //original-code:"memdebug.h"

/*
 * Forward declarations.
 */

static CURLcode dict_do(struct connectdata *conn, bool *done);

/*
 * DICT protocol handler.
 */

const struct Curl_handler Curl_handler_dict = {
  "DICT",                               /* scheme */
  ZERO_NULL,                            /* setup_connection */
  dict_do,                              /* do_it */
  ZERO_NULL,                            /* done */
  ZERO_NULL,                            /* do_more */
  ZERO_NULL,                            /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_getsock */
  ZERO_NULL,                            /* doing_getsock */
  ZERO_NULL,                            /* domore_getsock */
  ZERO_NULL,                            /* perform_getsock */
  ZERO_NULL,                            /* disconnect */
  ZERO_NULL,                            /* readwrite */
  PORT_DICT,                            /* defport */
  CURLPROTO_DICT,                       /* protocol */
  PROTOPT_NONE | PROTOPT_NOURLQUERY      /* flags */
};

static char *unescape_word(struct Curl_easy *data, const char *inputbuff)
{
  char *newp;
  char *dictp;
  char *ptr;
  size_t len;
  char ch;
  int olen=0;

  CURLcode result = Curl_urldecode(data, inputbuff, 0, &newp, &len, FALSE);
  if(!newp || result)
    return NULL;

  dictp = malloc(((size_t)len)*2 + 1); /* add one for terminating zero */
  if(dictp) {
    /* According to RFC2229 section 2.2, these letters need to be escaped with
       \[letter] */
    for(ptr = newp;
        (ch = *ptr) != 0;
        ptr++) {
      if((ch <= 32) || (ch == 127) ||
          (ch == '\'') || (ch == '\"') || (ch == '\\')) {
        dictp[olen++] = '\\';
      }
      dictp[olen++] = ch;
    }
    dictp[olen]=0;
  }
  free(newp);
  return dictp;
}

static CURLcode dict_do(struct connectdata *conn, bool *done)
{
  char *word;
  char *eword;
  char *ppath;
  char *database = NULL;
  char *strategy = NULL;
  char *nthdef = NULL; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode result=CURLE_OK;
  struct Curl_easy *data=conn->data;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];

  char *path = data->state.path;
  curl_off_t *bytecount = &data->req.bytecount;

  *done = TRUE; /* unconditionally */

  if(conn->bits.user_passwd) {
    /* AUTH is missing */
  }

  if(strncasecompare(path, DICT_MATCH, sizeof(DICT_MATCH)-1) ||
     strncasecompare(path, DICT_MATCH2, sizeof(DICT_MATCH2)-1) ||
     strncasecompare(path, DICT_MATCH3, sizeof(DICT_MATCH3)-1)) {

    word = strchr(path, ':');
    if(word) {
      word++;
      database = strchr(word, ':');
      if(database) {
        *database++ = (char)0;
        strategy = strchr(database, ':');
        if(strategy) {
          *strategy++ = (char)0;
          nthdef = strchr(strategy, ':');
          if(nthdef) {
            *nthdef = (char)0;
          }
        }
      }
    }

    if((word == NULL) || (*word == (char)0)) {
      infof(data, "lookup word is missing\n");
      word=(char *)"default";
    }
    if((database == NULL) || (*database == (char)0)) {
      database = (char *)"!";
    }
    if((strategy == NULL) || (*strategy == (char)0)) {
      strategy = (char *)".";
    }

    eword = unescape_word(data, word);
    if(!eword)
      return CURLE_OUT_OF_MEMORY;

    result = Curl_sendf(sockfd, conn,
                        "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                        "MATCH "
                        "%s "    /* database */
                        "%s "    /* strategy */
                        "%s\r\n" /* word */
                        "QUIT\r\n",

                        database,
                        strategy,
                        eword
                        );

    free(eword);

    if(result) {
      failf(data, "Failed sending DICT request");
      return result;
    }
    Curl_setup_transfer(conn, FIRSTSOCKET, -1, FALSE, bytecount,
                        -1, NULL); /* no upload */
  }
  else if(strncasecompare(path, DICT_DEFINE, sizeof(DICT_DEFINE)-1) ||
          strncasecompare(path, DICT_DEFINE2, sizeof(DICT_DEFINE2)-1) ||
          strncasecompare(path, DICT_DEFINE3, sizeof(DICT_DEFINE3)-1)) {

    word = strchr(path, ':');
    if(word) {
      word++;
      database = strchr(word, ':');
      if(database) {
        *database++ = (char)0;
        nthdef = strchr(database, ':');
        if(nthdef) {
          *nthdef = (char)0;
        }
      }
    }

    if((word == NULL) || (*word == (char)0)) {
      infof(data, "lookup word is missing\n");
      word=(char *)"default";
    }
    if((database == NULL) || (*database == (char)0)) {
      database = (char *)"!";
    }

    eword = unescape_word(data, word);
    if(!eword)
      return CURLE_OUT_OF_MEMORY;

    result = Curl_sendf(sockfd, conn,
                        "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                        "DEFINE "
                        "%s "     /* database */
                        "%s\r\n"  /* word */
                        "QUIT\r\n",
                        database,
                        eword);

    free(eword);

    if(result) {
      failf(data, "Failed sending DICT request");
      return result;
    }
    Curl_setup_transfer(conn, FIRSTSOCKET, -1, FALSE, bytecount,
                        -1, NULL); /* no upload */
  }
  else {

    ppath = strchr(path, '/');
    if(ppath) {
      int i;

      ppath++;
      for(i = 0; ppath[i]; i++) {
        if(ppath[i] == ':')
          ppath[i] = ' ';
      }
      result = Curl_sendf(sockfd, conn,
                          "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                          "%s\r\n"
                          "QUIT\r\n", ppath);
      if(result) {
        failf(data, "Failed sending DICT request");
        return result;
      }

      Curl_setup_transfer(conn, FIRSTSOCKET, -1, FALSE, bytecount, -1, NULL);
    }
  }

  return CURLE_OK;
}
#endif /*CURL_DISABLE_DICT*/
