#ifndef UTILS_H
#define UTILS_H

#include "str.h"

enum FileType {
	FILE_UNKNOWN = 0,
	FILE_REGULAR,
	FILE_DIRECTORY,
	FILE_SYMLINK,
};

String* utils_posix_base(const String *str);
enum FileType utils_posix_file_type(String *abspath);
String* utils_posix_path_abs(String *path);
StringArray* utils_posix_list_dir(String *path);

String* utils_win_base(const String *str);
enum FileType utils_win_file_type(String *abspath);
String* utils_posix_win_abs(String *path);
StringArray* utils_win_list_dir(String *path);

#endif /* UTILS_H */
