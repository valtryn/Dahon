// NOTE: not really sure what the side effects are if I define this.
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>

#include "str.h"
#include "utils.h"


String* utils_posix_base(const String *path)
{
	StringArray *sa = NULL;
	String sep;
	String *base = str_init();
	// return "." if the path is empty ""
	if (str_len(path) == 0) {
		base->data = malloc(sizeof(unsigned char));
		base->data[0] = '.';
		base->length = 1;
		goto exit2;
	}

	str_clone_from_cstr(&sep, "/");
	sa = str_split(path, &sep);
	if (sa->length == 1) {
		str_clone(base, path);
		goto exit1;
	}
	for (size_t i = sa->length; i > 0; i--) {
		if (str_array_at(sa, i-1)->length > 0) {
			str_clone(base, str_array_at(sa, i-1));
			break;
		}
	}
	if (str_len(base) == 0) {
		str_clone_from_cstr(base, "/");
	}
exit1:
	str_array_free(sa);
	str_clear(&sep);
exit2:
	return base;
}

enum FileType utils_posix_file_type(String *abspath)
{
	enum FileType ft = FILE_UNKNOWN;
	str_normalize(abspath);

	struct stat s;
	if (stat((const char *)str_view(abspath), &s) != 0) {
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
	String *abs = str_init();
	str_normalize(path);
	char *rp = realpath((const char *)str_view(path), NULL);
	if (rp == NULL) {
		str_free(abs);
		return NULL;
	}
	str_clone_from_cstr(abs, rp);
	free(rp);
	return abs;
}


StringArray* _utils_posix_list_dir_cap(String *path, int capacity)
{
	DIR *dir = NULL;
	struct dirent *entries;
	StringArray *sa = NULL;
	str_normalize(path);
	dir = opendir(str_view_cstr(path));
	if (dir == NULL) {
		return NULL;
	}
	
	sa = malloc(sizeof(StringArray));
	str_array_init(sa, capacity);
	while ((entries = readdir(dir)) != NULL) {
		String temp;
		str_clone_from_cstr(&temp, entries->d_name);
		str_array_append_move(sa, &temp);
	}
	closedir(dir);
	return sa;
}

StringArray* utils_posix_list_dir(String *path)
{
	return _utils_posix_list_dir_cap(path, 1<<8);
}
