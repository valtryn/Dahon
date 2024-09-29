// NOTE: not really sure what the side effects are if I define this.
#define _DEFAULT_SOURCE

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include "str.h"
#include "utils.h"

#define ABSPATH (1 << 0 )
#define ROOT    (1 << 1)
#define INF_DEPTH 0
#define LIMITED_DEPTH 1

String* utils_posix_base(const String *path)
{
	StringArray *sa = NULL;
	String *sep = NULL;
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
	if (str_len(base) == 0)
		base = str_clone_from_cstr("/");
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
* TODO: 
*	- optimize this piece of shit
*	- it uses too much memory
*	- the string_builder is probably the problem
*	- use dfs
 */
void utils_posix_list_dir(String* path, StringArray *sa, int depth)
{
	if (depth == 0)
		return;

	if (str_len(path) == 0) {
		printf("ERROR: cannot open path\n");
		return;
	}

	DIR *dir = NULL;
	struct dirent *entry;
	dir = opendir(str_view_cstr(path));
	if (!dir) {
		fprintf(stderr, "ERROR: cannot open %s: %s\n", str_view_cstr(path), strerror(errno));
		return;
	}
	while ((entry = readdir(dir)) != NULL) {
		String *name = str_clone_from_cstr(entry->d_name);
		if ((str_len(name) == 1 && memcmp(str_view(name), ".", 1) == 0) || ((str_len(name) == 2) && memcmp(str_view(name), "..", 2) == 0)) {
			str_free(name);
			continue;
		}

		StringBuilder sb;
		String *built_string;

		str_builder_init(&sb, 10);
		// TODO: this check wont handle "./././home". but its fine for now
		if ((str_len(path) == 1 && str_view(path)[0] == '.') || (str_len(path) == 2 && memcmp(str_view(path), "./", 2) == 0)) {
			str_write_string_move(&sb, name);
		} else {
			str_write_string(&sb, path);
			str_write_string_move(&sb, str_clone_from_cstr("/"));
			str_write_string_move(&sb, name);
		}

		str_init(&built_string);
		str_builder_to_string(&sb, built_string);
		switch (entry->d_type) {
			case DT_LNK:
			case DT_REG:
				str_array_append(sa, built_string);
				break;
			case DT_DIR:
				str_array_append(sa, built_string);
				utils_posix_list_dir(built_string, sa, (depth < 0) ? depth : depth -1 );
				break;
			case DT_UNKNOWN: {
						 struct stat s;
						 if (lstat(str_view_cstr(built_string), &s) != 0) {
							 fprintf(stderr, "ERROR: cannot open %s: %s\n", str_view_cstr(built_string), strerror(errno));
							 free(built_string);
							 str_builder_clear(&sb);
							 continue;
						 }
						 if (S_ISDIR(s.st_mode)) {
							 str_array_append(sa, built_string);
							 utils_posix_list_dir(built_string, sa, (depth < 0) ? depth : depth -1 );
						 } else {
							 str_array_append(sa, built_string);
						 }
					 }

		}
		free(built_string);
		str_builder_clear(&sb);
	}
	closedir(dir);
}
