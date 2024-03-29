/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#ifndef INCLUDE_commit_h__
#define INCLUDE_commit_h__

#include BLIK_LIBGIT2_U_git2__commit_h //original-code:"git2/commit.h"
#include "tree.h"
#include BLIK_LIBGIT2_U_repository_h //original-code:"repository.h"
#include "array.h"

#include <time.h>

struct git_commit {
	git_object object;

	git_array_t(git_oid) parent_ids;
	git_oid tree_id;

	git_signature *author;
	git_signature *committer;

	char *message_encoding;
	char *raw_message;
	char *raw_header;

	char *summary;
};

void git_commit__free(void *commit);
int git_commit__parse(void *commit, git_odb_object *obj);

#endif
