/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */

#ifndef INCLUDE_tree_cache_h__
#define INCLUDE_tree_cache_h__

#include BLIK_LIBGIT2_U_common_h //original-code:"common.h"
#include "pool.h"
#include BLIK_LIBGIT2_U_buffer_h //original-code:"buffer.h"
#include BLIK_LIBGIT2_U_git2__oid_h //original-code:"git2/oid.h"

typedef struct git_tree_cache {
	struct git_tree_cache **children;
	size_t children_count;

	ssize_t entry_count;
	git_oid oid;
	size_t namelen;
	char name[GIT_FLEX_ARRAY];
} git_tree_cache;

int git_tree_cache_write(git_buf *out, git_tree_cache *tree);
int git_tree_cache_read(git_tree_cache **tree, const char *buffer, size_t buffer_size, git_pool *pool);
void git_tree_cache_invalidate_path(git_tree_cache *tree, const char *path);
const git_tree_cache *git_tree_cache_get(const git_tree_cache *tree, const char *path);
int git_tree_cache_new(git_tree_cache **out, const char *name, git_pool *pool);
/**
 * Read a tree as the root of the tree cache (like for `git read-tree`)
 */
int git_tree_cache_read_tree(git_tree_cache **out, const git_tree *tree, git_pool *pool);
void git_tree_cache_free(git_tree_cache *tree);

#endif
