/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#include <git2/common.h>

#if !defined(GIT_WIN32) && !defined(NO_MMAP)

#include BLIK_LIBGIT2_U_map_h //original-code:"map.h"
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

int git__page_size(size_t *page_size)
{
	long sc_page_size = sysconf(_SC_PAGE_SIZE);
	if (sc_page_size < 0) {
		giterr_set_str(GITERR_OS, "Can't determine system page size");
		return -1;
	}
	*page_size = (size_t) sc_page_size;
	return 0;
}

int p_mmap(git_map *out, size_t len, int prot, int flags, int fd, git_off_t offset)
{
	int mprot = PROT_READ;
	int mflag = 0;

	GIT_MMAP_VALIDATE(out, len, prot, flags);

	out->data = NULL;
	out->len = 0;

	if (prot & GIT_PROT_WRITE)
		mprot |= PROT_WRITE;

	if ((flags & GIT_MAP_TYPE) == GIT_MAP_SHARED)
		mflag = MAP_SHARED;
	else if ((flags & GIT_MAP_TYPE) == GIT_MAP_PRIVATE)
		mflag = MAP_PRIVATE;
	else
		mflag = MAP_SHARED;

	out->data = mmap(NULL, len, mprot, mflag, fd, offset);

	if (!out->data || out->data == MAP_FAILED) {
		giterr_set(GITERR_OS, "Failed to mmap. Could not write data");
		return -1;
	}

	out->len = len;

	return 0;
}

int p_munmap(git_map *map)
{
	assert(map != NULL);
	munmap(map->data, map->len);

	return 0;
}

#endif

