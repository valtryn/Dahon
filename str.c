#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "str.h"
#include "ds.h"

#define COPY 0
#define MOVE 1
#define MOVEFREE 2

// TODO: handle memory allocation failure

#define str_normalize(buffer, length) \
do { \
	(buffer) = malloc(sizeof(unsigned char) * (length) + 1); \
	(buffer)[length] = '\0'; \
} while (0); 

// Initialize a heap allocated `String`
void _str_init(String **str)
{
	*str = malloc(sizeof(String));
	(*str)->data = NULL;
	(*str)->length = 0;
}

// Returns a clone of the buffer as a `String`
String* str_clone_from_buf(const void *src, size_t length)
{
	const unsigned char *buf = (const unsigned char *)src;
	String *str = NULL;
	_str_init(&str);
	if (length == 0) {
		return str;
	}
	str_normalize(str->data, length);
	memcpy(str->data, buf, length);
	str->length = length;
	return str;
}

String* str_clone_from_cstr(const char *src)
{
	return str_clone_from_buf(src, strlen(src));
}

// Appends a null-byte to a `String` then clones it as a `cstring`
char* str_clone_to_cstr(const String *src)
{
	if (src == NULL || src->data == NULL)
		return NULL;

	char *cstr = malloc(sizeof(char) * (src->length + 1));
	if (cstr == NULL)
		return NULL;

	memcpy(cstr, str_view(src), str_len(src));
	cstr[src->length] = '\0';
	return cstr;
}

// Returns a clone of a `String`
String* str_clone(const String *src)
{
	String *str = NULL;
	_str_init(&str);
	str_normalize(str->data, str_len(src));
	if (str->data == NULL)
		return NULL;
	memcpy(str->data, str_view(src), str_len(src));
	str->length = str_len(src);
	return str;
}

// Returns a concatenation of two strings
String* str_merge(String *lhs, String *rhs)
{
	String *str = NULL;
	_str_init(&str);
	size_t l = str_len(lhs);
	size_t r = str_len(rhs);
	str_normalize(str->data, l + r);
	memcpy(str->data, str_view(lhs), l);
	memcpy(str->data + l, str_view(rhs), r);
	str->length = l + r;
	return str;
}

// Concatenates an array of strings, inserting a separator between each, and returns the result as a new String
String* str_join(const StringArray *sa, const String *separator)
{
	StringBuilder sb = {0};
	String *str = NULL;
	_str_init(&str);
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
	str->data = clone.data;
	str->length = str_len(&clone);
	str_builder_reset(&sb);
	return str;
}

// Concatenates an array of strings and returns the result as a new String
String* str_concatenate(const StringArray *sa)
{
	return str_join(sa, NULL);
}

// Returns the first occurrence of substr within s and returns its byte offset. Returns -1 if not found.
int str_index(const String *str, const String *substr)
{
	if (str_len(substr) == 0 || str_len(str) == 0 || str_len(str) < str_len(substr))
		return -1;
	if (str_len(substr) == 1) {
		return str_index_byte(str, (const char)str_view(substr)[0]);
	}
	for (size_t i = 0; i <= str_len(str) - str_len(substr); i++) {
		if (str_view(str)[i] == str_view(substr)[0]) {
			if (memcmp(str_view(str)+ i, str_view(substr), str_len(substr)) == 0)
				return (i <= INT_MAX) ? (int)i : -1; // TODO: maybe fix this?
		}
	}
	return -1;
}

// Returns the last occurrence of substr within String `str` and returns its byte offset. Returns -1 if not found.
int str_last_index(const String *str, const String *substr)
{
	DynamicArray arr;
	dynamic_array_init(&arr, sizeof(size_t), str_len(str));
	size_t ret = str_count_indices(str, substr, &arr);
	if (ret == 0) {
		dynamic_array_clear(&arr);
		return -1;
	}

	int last_index = *(int*)dynamic_array_at(&arr, arr.length - 1);
	dynamic_array_clear(&arr);
	return last_index;
}

// Returns the first occurrence of the char `byte` within String `str` and returns its index. Returns -1 if not found.
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

// Returns the number of occurrence of String `substr` within String `str`.
size_t str_count(const String *str, const String *substr)
{
	return str_count_indices(str, substr, NULL);
}


size_t str_count_indices(const String *str, const String *substr, DynamicArray *arr)
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
					dynamic_array_append(arr, &i);
				ret++;
			}
		}
		return ret;
	}
	// TODO: maybe reuse code from str_index
	for (size_t i = 0; i <= str_len(str) - str_len(substr); i++) {
		if (str_view(str)[i] == str_view(substr)[0]) {
			if (memcmp(str_view(str) + i, str_view(substr), str_len(substr)) == 0) {
				if (arr != NULL)
					dynamic_array_append(arr, &i);
				ret++;
				i += str_len(substr) - 1;
			}
		}
	}
	return ret;
}

StringArray* str_split(const String *str, const String *separator)
{
	DynamicArray arr = {0};
	StringArray *sa = NULL;
	dynamic_array_init(&arr, sizeof(size_t), 1);
	size_t count = str_count_indices(str, separator, &arr);
	sa = malloc(sizeof(StringArray));
	if (count == 0) {
		str_array_init(sa, 1);
		str_array_append(sa, str);
		goto exit;
	}
	str_array_init(sa, count);
	// special case where the separator is ""
	if (count == str_len(str) + 1) {
		for (size_t i = 0; i < arr.length; i++) {
			String *temp = str_clone_from_buf(str_view(str) + i, i);
			str_array_append_move(sa, temp);
		}
		goto exit;
	}
	size_t prev = 0;
	for (size_t i = 0; i < arr.length; i++) {
		size_t data = *(size_t*)dynamic_array_at(&arr, i);
		String *temp = str_clone_from_buf(str_view(str) + prev, data - prev);
		str_array_append_move(sa, temp);
		prev = data + str_len(separator);
	}
	size_t last_elem = *(size_t*)dynamic_array_at(&arr, arr.length - 1);
	if (str_len(str) - last_elem + str_len(separator)  > 0) {
		String *temp = str_clone_from_buf(str_view(str) + last_elem + str_len(separator), str_len(str) - (last_elem + str_len(separator)));
		str_array_append_move(sa, temp);
	}
exit:
	dynamic_array_clear(&arr);
	return sa;
}

void str_trim_left_space(String *str)
{
	if (str_len(str) == 0) {
		return;
	}
	size_t idx = 0;
	for (size_t i = 0; i < str_len(str); i++) {
		if (!isspace(str_view(str)[i])) {
			idx = i;
			break;
		}
	}
	if (idx > 0) {
		memmove(str->data, str->data + idx, str_len(str) - idx);
		str->length = str_len(str) - idx;
	}
}

void str_trim_right_space(String *str)
{
	for (size_t i = str_len(str); i > 0; i--) {
		if (!isspace(str_view(str)[i - 1])) {
			str->length = i;
			break;
		}
	}
}

void str_trim_space(String *str) {
	str_trim_right_space(str);
	str_trim_left_space(str);
}

/* +---------------------------------- STRING ARRAY ----------------------------------+ */

// Initializes a dynamic string array with a specified capacity.
// NOTE: the caller is responsible for allocating the actual StringArray struct. maybe we should fix this?
void str_array_init(StringArray *sa, size_t capacity)
{
	assert(capacity > 0 && "ERROR: capacity must be > 0");
	sa->length = 0;
	sa->capacity = capacity;
	sa->string = calloc(capacity, sizeof(String));
}

static void _str_array_append(StringArray *sa, String *str, int opt)
{
	if (sa->length >= sa->capacity) {
		sa->capacity *= 2;
		sa->string = realloc(sa->string, sa->capacity * sizeof(String));
	}
	sa->string[sa->length].length = str->length;
	if (opt == COPY) {
		/*sa->string[sa->length].data = malloc(sizeof(unsigned char) * str->length);*/
		str_normalize(sa->string[sa->length].data, str_len(str));
		memcpy(sa->string[sa->length].data, str->data, str->length);
	} else if (opt == MOVE) {
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

// Appends a shallow copy of the underlying data of a `String` to the array and free the passed `String`
void str_array_append_move(StringArray *sa, String *str)
{
	_str_array_append(sa, str, MOVE);
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

/*size_t str_write_string_move_free(StringBuilder *sb, String *str)*/
/*{*/
/*	size_t len = str_len(str);*/
/*	str_array_append_move_free(&sb->arr, str);*/
/*	return len;*/
/*}*/

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
	sb->buffer = realloc(sb->buffer, accum + 1);
	sb->buffer[accum] = '\0';
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

inline const char* str_view_cstr(const String *str)
{
	return (const char *)str->data;
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
	if (str == NULL) {
		printf("error: str is NULL\n");
		return;
	}
	printf("string: %.*s | length: %ld\n", (int)str_len(str), str_view(str), str_len(str));
}
