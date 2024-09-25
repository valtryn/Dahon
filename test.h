#ifndef TEST_H
#define TEST_H

#include "str.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

#define ARRAY_LEN(arr) (sizeof((arr)) / sizeof((arr)[0]))

#define TEST_BEGIN() do { \
    printf("+---------------------------- %s ----------------------------+\n", __func__); \
} while(0)

#define PASSED(case) do { \
	printf("CASE %d: PASSED\n", (case));\
} while(0)

#define FAILED(case) do { \
	printf("CASE %d: FAILED: Line: %d\n", (case), __LINE__);\
} while(0)

void test_str_cstr_to_str(void)
{
	TEST_BEGIN();
	// stack
	char *str = "stack";
	String str_stack;
	str_clone_from_cstr(&str_stack, str);
	if (str_len(&str_stack) == strlen(str) && (memcmp(str_view(&str_stack), str, strlen(str)) == 0)) {
		PASSED(1);
	} else {
		FAILED(1);
	}
	str_clear(&str_stack);
	// heap
	char *strh = "heap";
	String *str_heap = str_init();
	str_clone_from_cstr(str_heap, strh);
	if (str_len(str_heap) == strlen(strh) && (memcmp(str_view(str_heap), strh, strlen(strh)) == 0)) {
		PASSED(2);
	} else {
		FAILED(2);
	}
	str_free(str_heap);
}

void test_str_array(void)
{
	StringArray sa = {0};
	str_array_init(&sa, 1);
	/*const char *separators[] = { "a", "bb", "cc", "abbcccc", "abbccccx" };*/
	/*const char *separators[] = { "a" };*/
	/*for (size_t i = 0; i < sizeof(separators)/sizeof(separators[0]); i++) {*/
	char *cstr = "string";
	for (size_t i = 0; i < 1000000; i++) {
		String str;
		str_clone_from_cstr(&str, cstr);
		/*str_clone_from_cstr(&str, separators[i]);*/
		str_array_append_move(&sa, &str);
	}
	/*str_array_remove_at_unordered(&sa, 0);*/
	str_array_reset(&sa);
	/*String str;*/
	/*str_clone_from_cstr(&str, separators[0]);*/
	/*str_array_append_move(&sa, &str);*/
	for (size_t i = 0; i < sa.length; i++) {
		str_dbg_print(str_array_at(&sa, i));
	}
	str_array_clear(&sa);
}

void test_str_builder(void)
{
	const char *separators[] = { "foo", "bar", "baz" };
	StringBuilder *sb = malloc(sizeof(StringBuilder));
	str_builder_init(sb, 1);
	for (size_t i = 0; i < sizeof(separators)/sizeof(separators[0]); i++) {
		String str;
		str_clone_from_cstr(&str, separators[i]);
		str_write_string_move(sb, &str);
	}
	String built;
	str_builder_to_string(sb, &built);
	str_dbg_print(&built);
	printf("len: %ld\n", str_len(&built));
	str_builder_free(sb);

}

void test_str_normalize(void)
{
	String str;
	str_clone_from_cstr(&str, "string");
	str_normalize(&str);
	printf("data: %s- len: %ld\n", str_view(&str), strlen((const char *)str_view(&str)));
	str_clear(&str);
}

void test_str_merge(void)
{
	String str;
	String foo;
	String bar;
	str_clone_from_cstr(&foo, "foo");
	str_clone_from_cstr(&bar, "bar");
	str_merge(&str, &foo, &bar);
	str_dbg_print(&str);
	str_clear(&str);
	str_clear(&foo);
	str_clear(&bar);
}

void test_str_concatenate(void)
{
	const char *values[] = { "foo", "bar", "baz" };
	StringArray sa;
	str_array_init(&sa, 1);
	for (size_t i = 0; i < ARRAY_LEN(values); i++) {
		String str;
		str_clone_from_cstr(&str, values[i]);
		str_array_append_move(&sa, &str);
	}

	/*for (size_t i = 0; i < sa.length; i++) {*/
	/*	str_dbg_print(str_array_at(&sa, i));*/
	/*}*/

	String sep;
	str_clone_from_cstr(&sep, "+---+");
	String built;
	str_join(&built, &sa, &sep);
	str_dbg_print(&built);
	
	str_array_clear(&sa);
	str_clear(&built);
	str_clear(&sep);
}

void test_str_index(void)
{
	String str;
	String substr;
	str_clone_from_cstr(&str, "string");
	str_clone_from_cstr(&substr, "ing");
	int ret = str_index(&str, &substr);
	printf("ret: %d\n", ret);
	str_clear(&str);
	str_clear(&substr);
}
void test_str_split(void)
{
	FILE* fp = fopen("./benchmark/shakespeare.txt", "r");

	// Get the file size
	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	rewind(fp);

	// Allocate memory for the buffer
	char* buffer = (char*)malloc(file_size + 1);

	// Read the file contents into the buffer
	size_t bytes_read = fread(buffer, 1, file_size, fp);

	Array arr = {0};
	array_init(&arr, sizeof(size_t), 1<<8);
	String str;
	String substr;
	/*str_clone_from_cstr(&str, "--dog--cat--cow--s");*/
	str_clone_from_buf(&str, buffer, bytes_read);
	str_clone_from_cstr(&substr, "the");
	StringArray *sa = str_split(&str, &substr);
	/*for (size_t i = 0; i < sa->length; i++) {*/
	/*	str_dbg_print(str_array_at(sa, i));*/
	/*}*/
	str_array_free(sa);
	array_clear(&arr);
	str_clear(&str);
	str_clear(&substr);
	free(buffer);
	fclose(fp);
}

void test_str_trim_space(void)
{
	String str;
	str_clone_from_cstr(&str, "xyz");
	str_dbg_print(&str);
	str_trim_space(&str);
	str_dbg_print(&str);
	str_clear(&str);

}

void test_str_last_index(void)
{
	String str;
	String substr;
	str_clone_from_cstr(&str, "test");
	str_clone_from_cstr(&substr, "tt");
	str_dbg_print(&str);
	int index = str_last_index(&str, &substr);
	printf("last index: %d\n", index);
	str_clear(&str);
	str_clear(&substr);
}
#endif /* TEST_H */
