#ifndef STR_H
#define STR_H

#include <stdlib.h>
#include "utils.h"

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

String* str_init(void);
// TODO: make a heap variant of these functions
void str_clone_from_buf(String *dest, const void *src_buf, size_t length);
void str_clone_from_cstr(String *dest, const char *src_cstr);
void str_clone_to_cstr(char **dest, const String *src);
void str_clone(String *dest, const String *src);
void str_merge(String *dest, String *lhs, String *rhs);
void str_join(String *dest, const StringArray *sa, const String *separator);
void str_concatenate(String *dest, const StringArray *sa);
int str_index(const String *str, const String *substr);
int str_index_byte(const String *str, const char byte);
size_t str_count(const String *str, const String *substr);
size_t str_count_indices(const String *str, const String *substr, Array *arr);
StringArray* str_split(const String *str, const String *separator);
void str_trim_right_space(String *str);
void str_trim_left_space(String *str);
void str_trim_space(String *str);
/* +---------------------------------- STRING ARRAY ----------------------------------+ */
void str_array_init(StringArray *sa, size_t capacity);
void str_array_append(StringArray *sa, const String *str);
void str_array_append_move(StringArray *sa, String *str);
void str_array_append_move_free(StringArray *sa, String *str);
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
size_t str_write_string_move_free(StringBuilder *sb, String *str);
void str_builder_to_string(StringBuilder *sb, String *dest);
void str_builder_grow(StringBuilder *sb, size_t size);
void str_builder_reset(StringBuilder *sb);
void str_builder_clear(StringBuilder *sb);
void str_builder_free(StringBuilder *sb);

/* +---------------------------------- UTIL FUNC ----------------------------------+ */
const unsigned char* str_view(const String *str);
void str_reset(String *str);
void str_clear(String *str);
void str_free(String *str);
size_t str_len(const String *str);
void str_normalize(String *str);

/* +---------------------------------- DEBUG FUNC ----------------------------------+ */
void str_dbg_print(String *str);

/* +---------------------------------- UNDER DEVELOPMENT FUNC ----------------------------------+ */
int last_index_byte(const String *Str, const char c);
#endif /* STR_H */
