#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "str.h"
#include "utils.h"

#define COPY 0
#define MOVE 1
#define MOVEFREE 2

// Initialize a heap allocated `String`
String* str_init(void)
{
	String *str = malloc(sizeof(String));
	str->data = NULL;
	str->length = 0;
	return str;
}

// Converts a memory buffer into a `String`
void str_clone_from_buf(String *dest, const void *src_buf, size_t length)
{
	const unsigned char *buf = (const unsigned char *)src_buf;
	if (length == 0) {
		dest->data = NULL;
		dest->length = 0;
		return;
	}
	dest->data = malloc(sizeof(unsigned char) * length);
	if (dest->data == NULL) {
		dest->length = 0;
		return;
	}
	memcpy(dest->data, buf, length);
	dest->length = length;
	return;
}

// Clones a cstring `src_cstr` as a `String`
void str_clone_from_cstr(String *dest, const char *src_cstr)
{
	str_clone_from_buf(dest, src_cstr, strlen(src_cstr));
}

// Appends a null-byte to a `String` then clones it as a `cstring`
void str_clone_to_cstr(char **dest, const String *src)
{
	if (src == NULL || src->data == NULL)
		return;

	char *cstr = malloc(sizeof(char) * (src->length + 1));
	if (cstr == NULL)
		return;

	memcpy(cstr, str_view(src), str_len(src));
	cstr[src->length] = '\0';
	*dest = cstr;
	return;
}

// Clones a `String`
void str_clone(String *dest, const String *src)
{
	dest->data = malloc(sizeof(unsigned char) * str_len(src));
	if (dest->data == NULL)
		return;
	memcpy(dest->data, str_view(src), str_len(src));
	dest->length = str_len(src);
}

void str_merge(String *dest, String *lhs, String *rhs)
{
	size_t l = str_len(lhs);
	size_t r = str_len(rhs);
	dest->data = malloc(sizeof(unsigned char) *(str_len(lhs) + str_len(rhs)));
	memcpy(dest->data, str_view(lhs), l);
	memcpy(dest->data + l, str_view(rhs), r);
	dest->length = l + r;
}

void str_join(String *dest, const StringArray *sa, const String *separator)
{
	StringBuilder sb = {0};
	str_builder_init(&sb, str_array_total_str_len(sa));
	for (size_t i = 0; i < sa->length; i++) {
		// writes a copy of passed string
		str_write_string(&sb, str_array_at(sa, i));
		if (separator != NULL && str_len(separator) > 0)
			str_write_string(&sb, separator);
	}
	if (separator != NULL && str_len(separator) > 0)
		str_array_shrink(&sb.arr, sb.arr.length - 1);
	String clone = {0};
	str_builder_to_string(&sb, &clone);
	dest->data = (unsigned char *)str_view(&clone);
	dest->length = str_len(&clone);
	str_builder_reset(&sb);
}

void str_concatenate(String *dest, const StringArray *sa)
{
	str_join(dest, sa, NULL);
}

int str_index(const String *str, const String *substr)
{
	if (str_len(substr) == 0 || str_len(str) == 0 || str_len(str) < str_len(substr))
		return -1;
	if (str_len(substr) == 1) {
		return str_index_byte(str, (const char)str_view(substr)[0]);
	}
	for (size_t i = 0; i <= str_len(str) - str_len(substr); i++) {
		const unsigned char *src_d = str_view(str);
		const unsigned char *sub_d = str_view(substr);
		if (src_d[i] == sub_d[0]) {
			if (memcmp(src_d + i, sub_d, str_len(substr)) == 0)
				return (i <= INT_MAX) ? (int)i : -1; // TODO: maybe fix this?
		}
	}
	return -1;
}

int str_index_byte(const String *str, const char byte)
{
	if (str_len(str) == 0)
		return -1;
	for (size_t i = 0; i < str_len(str); i++) {
		if (str_view(str)[i] == (const unsigned char)byte)
			return (i <= INT_MAX) ? (int)i : -1; // TODO: maybe fix this?
	}
	return -1;
}

size_t str_count(const String *str, const String *substr)
{
	return str_count_indices(str, substr, NULL);
}

size_t str_count_indices(const String *str, const String *substr, Array *arr)
{
	size_t ret = 0;
	if (str_len(str) < str_len(substr))
		return ret;
	if (str_len(substr) == 0) {
		return str_len(str) + 1;
	}
	if (str_len(substr) == 1) {
		for (size_t i = 0; i < str_len(str); i++) {
			if (str_view(str)[i] == str_view(substr)[0]) {
				if (arr != NULL)
					array_append(arr, &i);
				ret++;
			}
		}
		return ret;
	}
	// TODO: maybe reuse code from str_index
	for (size_t i = 0; i <= str_len(str) - str_len(substr); i++) {
		const unsigned char *src_d = str_view(str);
		const unsigned char *sub_d = str_view(substr);
		if (src_d[i] == sub_d[0]) {
			if (memcmp(src_d + i, sub_d, str_len(substr)) == 0) {
				if (arr != NULL)
					array_append(arr, &i);
				ret++;
				i += str_len(substr) - 1;
			}
		}
	}
	return ret;
}

StringArray* str_split(const String *str, const String *separator)
{
	Array arr = {0};
	StringArray *sa = NULL;
	array_init(&arr, sizeof(size_t), 1);
	size_t count = str_count_indices(str, separator, &arr);
	if (count == 0) {
		goto exit;
	}
	sa = malloc(sizeof(StringArray));
	str_array_init(sa, count);
	// special case where the separator is ""
	if (count == str_len(str) + 1) {
		for (size_t i = 0; i < arr.length; i++) {
			String temp;
			str_clone_from_buf(&temp, str_view(str) + i, i);
			str_array_append_move(sa, &temp);
		}
		goto exit;
	}
	size_t prev = 0;
	for (size_t i = 0; i < arr.length; i++) {
		size_t data = *(size_t*)array_at(&arr, i);
		String temp;
		str_clone_from_buf(&temp, str_view(str) + prev, data - prev);
		str_array_append_move(sa, &temp);
		prev = data + str_len(separator);
	}
	size_t last_elem = *(size_t*)array_at(&arr, arr.length - 1);
	if (str_len(str) - last_elem + str_len(separator)  > 0) {
		/*printf("size: %ld | strlen: %ld | last: %ld\n", str_len(str) - last_elem, str_len(str), last_elem);*/
		String temp;
		str_clone_from_buf(&temp, str_view(str) + last_elem + str_len(separator), str_len(str) - (last_elem + str_len(separator)));
		str_array_append_move(sa, &temp);
	}
exit:
	array_clear(&arr);
	return sa;
}

/* +---------------------------------- STRING ARRAY ----------------------------------+ */

// Initializes a dynamic string array with a specified capacity
void str_array_init(StringArray *sa, size_t capacity)
{
	assert(capacity > 0 && "ERROR: capacity must be > 0");
	sa->length = 0;
	sa->capacity = capacity;
	sa->string = calloc(capacity, sizeof(String));
}

void _str_array_append(StringArray *sa, String *str, int opt)
{
	if (sa->length >= sa->capacity) {
		sa->capacity *= 2;
		sa->string = realloc(sa->string, sa->capacity * sizeof(String));
	}
	sa->string[sa->length].length = str->length;
	if (opt == COPY) {
		sa->string[sa->length].data = malloc(sizeof(unsigned char) * str->length);
		memcpy(sa->string[sa->length].data, str->data, str->length);
	} else if (opt == MOVE) {
		sa->string[sa->length].data = str->data;
	} else if (opt == MOVEFREE) {
		sa->string[sa->length].data = str->data;
		free(str);
		str = NULL;
	}
	sa->length++;
}

// Appends a deep copy of a `String` to the array
void str_array_append(StringArray *sa, const String *str)
{
	_str_array_append(sa, (String *)str, COPY);
}

// Appends a shallow copy of the underlying data of a `String` to the array
void str_array_append_move(StringArray *sa, String *str)
{
	_str_array_append(sa, str, MOVE);
}

// Appends a shallow copy of the underlying data of a `String` to the array and free the passed `String`
void str_array_append_move_free(StringArray *sa, String *str)
{
	_str_array_append(sa, str, MOVEFREE);
}

String* str_array_at(const StringArray *sa, size_t index)
{
	return &sa->string[index];
}

void str_array_reset(StringArray *sa)
{
	str_array_shrink(sa, 0);
	sa->length = 0;
}

void str_array_clear(StringArray *sa)
{
	if (sa != NULL) {
		str_array_shrink(sa, 0);
		free(sa->string);
		sa->length = 0;
		sa->capacity = 0;
		sa->string = NULL;
	}
}

void str_array_free(StringArray *sa)
{
	if (sa != NULL) {
		str_array_clear(sa);
		free(sa);
	}
}

void str_array_shrink(StringArray *sa, size_t size)
{
	if (size >= sa->length)
		return;

	size_t diff = sa->length - size;
	for (size_t i = 0; i < diff; i++) {
		str_clear(&sa->string[i + size]);
	}
	sa->length = size;
}

// Remove an element from the array without preserving the order
void str_array_remove_at_unordered(StringArray *sa, size_t index)
{
	if (index < sa->length) {
		if (sa->length > 1 && sa->length - 1 != index) {
			unsigned char *temp = sa->string[index].data;
			size_t temp_len = str_len(&sa->string[index]);
			sa->string[index].data = sa->string[sa->length-1].data;
			sa->string[index].length = sa->string[sa->length-1].length;
			sa->string[sa->length-1].data = temp;
			sa->string[sa->length-1].length = temp_len;
			str_array_shrink(sa, sa->length - 1);
		} else {
			str_array_shrink(sa, sa->length - 1);
		}
	}
}

size_t str_array_total_str_len(const StringArray *sa)
{
	if (sa == NULL)
		return 0;

	size_t ret = 0;
	for (size_t i = 0; i < sa->length; i++) {
		ret += sa->string[i].length;
	}
	return ret;
}

/* +---------------------------------- STRING BUILDER ----------------------------------+ */
// TODO: maybe rewrite StringBuilder so that builder_to_string() would be an 0(1) operation

void str_builder_init(StringBuilder *sb, size_t capacity)
{

	assert(capacity > 0 && "ERROR: capacity must be > 0");
	sb->length = 0;
	sb->capacity = capacity;
	sb->buffer = malloc(sizeof(unsigned char) * capacity);
	// TODO: change initial capacity because using string capacity might be too huge
	str_array_init(&sb->arr, capacity);
}

size_t str_write_string(StringBuilder *sb, const String *str)
{
	str_array_append(&sb->arr, str);
	return str_len(str);
}

size_t str_write_string_move(StringBuilder *sb, String *str)
{
	str_array_append_move(&sb->arr, str);
	return str_len(str);
}

size_t str_write_string_move_free(StringBuilder *sb, String *str)
{
	size_t len = str_len(str);
	str_array_append_move_free(&sb->arr, str);
	return len;
}

void str_builder_grow(StringBuilder *sb, size_t size)
{
	if (size <= sb->length)
		return;
	sb->buffer = realloc(sb->buffer, sizeof(unsigned char) * size);
	sb->capacity = size;
}

void str_builder_to_string(StringBuilder *sb, String *dest)
{
	size_t total_len = str_array_total_str_len(&sb->arr);
	if (total_len > sb->capacity)
		str_builder_grow(sb, total_len);
	size_t accum = 0;
	for (size_t i = 0; i < sb->arr.length; i++) {
		String *elem = str_array_at(&sb->arr, i);
		size_t len = str_len(elem);
		memcpy(sb->buffer + accum, str_view(elem), len);
		accum += len;
	}
	dest->data = sb->buffer;
	dest->length = accum;
}

void str_builder_reset(StringBuilder *sb)
{
	sb->length = 0;
	str_array_clear(&sb->arr);
}

void str_builder_clear(StringBuilder *sb)
{
	if (sb != NULL) {
		free(sb->buffer);
		str_array_clear(&sb->arr);
		sb->buffer = NULL;
	}
}

void str_builder_free(StringBuilder *sb)
{
	str_builder_clear(sb);
	free(sb);
}

/* +---------------------------------- UTIL FUNC ----------------------------------+ */
inline const unsigned char* str_view(const String *str)
{
	return str->data;
}

// Appends a null terminator to make the string compatible with existing C string libraries.
void str_normalize(String *str)
{
	if (str != NULL && str->data != NULL) {
		str->data = realloc(str->data, str->length + 1);
		str->data[str->length] = '\0';
	}
}

inline void str_reset(String *str)
{
	if (str != NULL)
		str->length = 0;
}

inline void str_clear(String *str)
{
	if (str != NULL) {
		free(str->data);
		str->length = 0;
		str->data = NULL;
	}
}

inline void str_free(String *str)
{
	if (str != NULL) {
		str_clear(str);
		free(str);
	}
}

inline size_t str_len(const String *str)
{
	return str->length;
}



/* +---------------------------------- DEBUG FUNC ----------------------------------+ */
void str_dbg_print(String *str)
{
	printf("string: %.*s | length: %ld\n", (int)str_len(str), str_view(str), str_len(str));
}
