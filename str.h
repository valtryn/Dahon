#ifndef STR_H
#define STR_H

#include <stdlib.h>
#include "ds.h"

typedef struct String String;
typedef struct StringArray StringArray;
typedef struct StringBuilder StringBuilder;

struct String {
	size_t length;
	unsigned char *data;
};

struct StringArray {
	size_t length;
	size_t capacity;
	String *string;
};

struct StringBuilder {
	unsigned char *buffer;
	size_t length;
	size_t capacity;
	StringArray arr;
};

void str_init(String **str);
String* str_clone_from_buf(const void *src, size_t length);
String* str_clone_from_cstr(const char *src);
char*   str_clone_to_cstr(const String *src);
String* str_clone(const String *src);
String* str_merge(String *lhs, String *rhs);
String* str_join(const StringArray *sa, const String *separator);
String* str_concatenate(const StringArray *sa);
int str_index(const String *str, const String *substr);
int str_index_byte(const String *str, const char byte);
size_t str_count(const String *str, const String *substr);
size_t str_count_indices(const String *str, const String *substr, DynamicArray *arr);
StringArray* str_split(const String *str, const String *separator);
void str_trim_right_space(String *str);
void str_trim_left_space(String *str);
void str_trim_space(String *str);
/* +---------------------------------- STRING ARRAY ----------------------------------+ */
void str_array_init(StringArray *sa, size_t capacity);
void str_array_append(StringArray *sa, const String *str);
void str_array_append_move(StringArray *sa, String *str);
String* str_array_at(const StringArray *sa, size_t index);
void str_array_remove_at_unordered(StringArray *sa, size_t index);
void str_array_reset(StringArray *sa);
void str_array_clear(StringArray *sa);
void str_array_free(StringArray *sa);
void str_array_shrink(StringArray *sa, size_t size);
size_t str_array_total_str_len(const StringArray *sa);
int str_last_index(const String *str, const String *substr);

/* +---------------------------------- STRING BUILDER ----------------------------------+ */
void str_builder_init(StringBuilder *sb, size_t capacity);
size_t str_write_string(StringBuilder *sb, const String* str);
size_t str_write_string_move(StringBuilder *sb, String* str);
void str_builder_to_string(StringBuilder *sb, String *dest);
void str_builder_grow(StringBuilder *sb, size_t size);
void str_builder_reset(StringBuilder *sb);
void str_builder_clear(StringBuilder *sb);
void str_builder_free(StringBuilder *sb);

/* +---------------------------------- UTIL FUNC ----------------------------------+ */
const unsigned char* str_view(const String *str);
const char* str_view_cstr(const String *str);
void str_clear(String *str);
void str_free(String *str);
size_t str_len(const String *str);

/* +---------------------------------- DEBUG FUNC ----------------------------------+ */
void str_dbg_print(String *str);

/* +---------------------------------- UNDER DEVELOPMENT FUNC ----------------------------------+ */
int last_index_byte(const String *Str, const char c);
#endif /* STR_H */
