/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#ifndef INCLUDE_git_describe_h__
#define INCLUDE_git_describe_h__

#include BLIK_LIBGIT2_U_common_h //original-code:"common.h"
#include "types.h"
#include BLIK_LIBGIT2_U_buffer_h //original-code:"buffer.h"

/**
 * @file git2/describe.h
 * @brief Git describing routines
 * @defgroup git_describe Git describing routines
 * @ingroup Git
 * @{
 */
GIT_BEGIN_DECL

/**
 * Reference lookup strategy
 *
 * These behave like the --tags and --all optios to git-describe,
 * namely they say to look for any reference in either refs/tags/ or
 * refs/ respectively.
 */
typedef enum {
	GIT_DESCRIBE_DEFAULT,
	GIT_DESCRIBE_TAGS,
	GIT_DESCRIBE_ALL,
} git_describe_strategy_t;

/**
 * Describe options structure
 *
 * Initialize with `GIT_DESCRIBE_OPTIONS_INIT` macro to correctly set
 * the `version` field.  E.g.
 *
 *		git_describe_options opts = GIT_DESCRIBE_OPTIONS_INIT;
 */
typedef struct git_describe_options {
	unsigned int version;

	unsigned int max_candidates_tags; /** default: 10 */
	unsigned int describe_strategy; /** default: GIT_DESCRIBE_DEFAULT */
	const char *pattern;
	/**
	 * When calculating the distance from the matching tag or
	 * reference, only walk down the first-parent ancestry.
	 */
	int only_follow_first_parent;
	/**
	 * If no matching tag or reference is found, the describe
	 * operation would normally fail. If this option is set, it
	 * will instead fall back to showing the full id of the
	 * commit.
	 */
	int show_commit_oid_as_fallback;
} git_describe_options;

#define GIT_DESCRIBE_DEFAULT_MAX_CANDIDATES_TAGS 10
#define GIT_DESCRIBE_DEFAULT_ABBREVIATED_SIZE 7

#define GIT_DESCRIBE_OPTIONS_VERSION 1
#define GIT_DESCRIBE_OPTIONS_INIT { \
	GIT_DESCRIBE_OPTIONS_VERSION, \
	GIT_DESCRIBE_DEFAULT_MAX_CANDIDATES_TAGS, \
}

GIT_EXTERN(int) git_describe_init_options(git_describe_options *opts, unsigned int version);

/**
 * Options for formatting the describe string
 */
typedef struct {
	unsigned int version;

	/**
	 * Size of the abbreviated commit id to use. This value is the
	 * lower bound for the length of the abbreviated string. The
	 * default is 7.
	 */
	unsigned int abbreviated_size;

	/**
	 * Set to use the long format even when a shorter name could be used.
	 */
	int always_use_long_format;

	/**
	 * If the workdir is dirty and this is set, this string will
	 * be appended to the description string.
	 */
	char *dirty_suffix;
} git_describe_format_options;

#define GIT_DESCRIBE_FORMAT_OPTIONS_VERSION 1
#define GIT_DESCRIBE_FORMAT_OPTIONS_INIT { \
		GIT_DESCRIBE_FORMAT_OPTIONS_VERSION,   \
		GIT_DESCRIBE_DEFAULT_ABBREVIATED_SIZE, \
 }

GIT_EXTERN(int) git_describe_init_format_options(git_describe_format_options *opts, unsigned int version);

typedef struct git_describe_result git_describe_result;

/**
 * Describe a commit
 *
 * Perform the describe operation on the given committish object.
 *
 * @param result pointer to store the result. You must free this once
 * you're done with it.
 * @param committish a committish to describe
 * @param opts the lookup options
 */
GIT_EXTERN(int) git_describe_commit(
	git_describe_result **result,
	git_object *committish,
	git_describe_options *opts);

/**
 * Describe a commit
 *
 * Perform the describe operation on the current commit and the
 * worktree. After peforming describe on HEAD, a status is run and the
 * description is considered to be dirty if there are.
 *
 * @param result pointer to store the result. You must free this once
 * you're done with it.
 * @param repo the repository in which to perform the describe
 * @param opts the lookup options
 */
GIT_EXTERN(int) git_describe_workdir(
	git_describe_result **out,
	git_repository *repo,
	git_describe_options *opts);

/**
 * Print the describe result to a buffer
 *
 * @param result the result from `git_describe_commit()` or
 * `git_describe_workdir()`.
 * @param opt the formatting options
 */
GIT_EXTERN(int) git_describe_format(
	git_buf *out,
	const git_describe_result *result,
	const git_describe_format_options *opts);

/**
 * Free the describe result.
 */
GIT_EXTERN(void) git_describe_result_free(git_describe_result *result);

/** @} */
GIT_END_DECL

#endif
