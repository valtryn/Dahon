// NOTE: not really sure what the side effects are if I define this.
#define _XOPEN_SOURCE 500

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>

#include "str.h"
#include "utils.h"

#define ABSPATH (1 << 0 )
#define ROOT    (1 << 1)
#define INF_DEPTH 0
#define LIMITED_DEPTH 1

String* utils_posix_base(const String *path)
{
	StringArray *sa = NULL;
	String *sep;
	String *base = NULL;
	// return "." if the path is empty ""
	if (str_len(path) == 0) {
		base->data = malloc(sizeof(unsigned char));
		base->data[0] = '.';
		base->length = 1;
		goto exit2;
	}

	sep = str_clone_from_cstr("/");
	sa = str_split(path, sep);
	if (sa->length == 1) {
		base = str_clone(path);
		goto exit1;
	}
	for (size_t i = sa->length; i > 0; i--) {
		if (str_array_at(sa, i-1)->length > 0) {
			base = str_clone(str_array_at(sa, i-1));
			break;
		}
	}
	if (str_len(base) == 0) {
		base = str_clone_from_cstr("/");
	}
exit1:
	str_array_free(sa);
	str_free(sep);
exit2:
	return base;
}

enum FileType utils_posix_file_type(String *path)
{
	enum FileType ft = FILE_UNKNOWN;
	if (path == NULL)
		return ft;

	struct stat s;
	if (stat((const char *)str_view(path), &s) != 0) {
		// TODO: print error log
		goto exit;
	}

	if (S_ISREG(s.st_mode)) {
		ft = FILE_REGULAR;
		goto exit;
	} else if(S_ISDIR(s.st_mode)) {
		ft = FILE_DIRECTORY;
		goto exit;
	} else if(S_ISLNK(s.st_mode)) {
		// TODO: test this
		ft = FILE_SYMLINK;
		goto exit;
	}
exit:
	return ft;
}

String* utils_posix_path_abs(String *path)
{
	if (path == NULL)
		return NULL;

	String *abs = malloc(sizeof(String));
	char *rp = NULL;
	rp = realpath(str_view_cstr(path), NULL);
	if (rp == NULL) {
		str_free(abs);
		return NULL;
	}
	abs->data = (unsigned char *)rp;
	abs->length = strlen(rp);
	return abs;
}

/*
 * TODO: optimize this piece of shit
 * - it uses too much memory
 * - the string_builder is probably the problem
 */
void utils_posix_list_dir(String* path, StringArray *sa, int depth)
{
	if (depth == 0)
		return;

	DIR *dir = NULL;
	struct dirent *entries;
	dir = opendir(str_view_cstr(path));
	if (!dir)
		return;

	while ((entries = readdir(dir)) != NULL) {
		String *entry = str_clone_from_cstr(entries->d_name);
		if (memcmp(str_view(entry), ".", 1) == 0 || memcmp(str_view(entry), "..", 2) == 0) {
			str_free(entry);
			continue;
		}
		StringBuilder sb;
		str_builder_init(&sb, 100);
		str_write_string(&sb, path);
		str_write_string_move(&sb, str_clone_from_cstr("/"));
		str_write_string_move(&sb, entry);
		String *real_entry = malloc(sizeof(String));
		str_builder_to_string(&sb, real_entry);
		struct stat s;
		if (stat(str_view_cstr(real_entry), &s) != 0) {
			free(real_entry);
			str_builder_clear(&sb);
			continue;
		}
		if(S_ISDIR(s.st_mode)) {
			str_array_append(sa, real_entry);
			utils_posix_list_dir(real_entry, sa, (depth < 0) ? depth : depth -1 );
		} else {
			str_array_append(sa, real_entry);
		}
		free(real_entry);
		str_builder_clear(&sb);
	}
	closedir(dir);
}
