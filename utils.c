#include "str.h"
#include <stdio.h>
#include <stdlib.h>

String* path_posix_base(const String *path)
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
