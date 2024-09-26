#ifndef DS_H
#define DS_H

#include <stdlib.h>

/*+----------------------------- DYNAMIC ARRAY ------------------------+*/

typedef struct DynamicArray DynamicArray;

struct DynamicArray {
	size_t length;
	size_t capacity;
	size_t item_size;
	void *data;
};

void dynamic_array_init(DynamicArray *arr, size_t item_size, size_t capacity);
void* dynamic_array_at(DynamicArray *arr, size_t index);
void dynamic_array_append(DynamicArray *arr, void *data);
void dynamic_array_clear(DynamicArray *arr);
void dynamic_array_clear_custom(DynamicArray *arr, void (*custom_free)(void *));
void dynamic_array_free(DynamicArray *arr);
void dynamic_array_free_custom(DynamicArray *arr, void (*custom_free)(void *));

#endif /* DS_H */
