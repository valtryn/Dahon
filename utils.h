#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

/*+----------------------------- DYNAMIC ARRAY ------------------------+*/
typedef struct Array Array;

struct Array {
	size_t length;
	size_t capacity;
	size_t item_size;
	void *data;
};

void array_init(Array *arr, size_t item_size, size_t capacity);
void* array_at(Array *arr, size_t index);
void array_append(Array *arr, void *data);
void array_clear_custom(Array *arr, void (*custom_destroy)(void *));
void array_clear(Array *arr);
void array_destroy_custom(Array *arr, void (*custom_destroy)(void *));
void array_destroy(Array *arr);

#define array_append_string(arr, str)\
	do {\
		String *temp = strings_copy((str));\
		array_append(&(arr), &temp);\
	} while(0)
#endif /* UTILS_H */


