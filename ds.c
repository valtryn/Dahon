#include <string.h>

#include "ds.h"

/*+----------------------------- DYNAMIC ARRAY ------------------------+*/

void dynamic_array_init(DynamicArray *arr, size_t item_size, size_t capacity)
{
	arr->length = 0;
	arr->capacity = capacity;
	arr->item_size = item_size;
	arr->data = malloc(item_size * capacity);
}

void dynamic_array_append(DynamicArray *arr, void *data)
{
	if (arr->length >= arr->capacity) {
		arr->capacity *= 2;
		arr->data = realloc(arr->data, arr->item_size * arr->capacity);
	}
	unsigned char *element = (unsigned char*)arr->data + (arr->length * arr->item_size);
	memcpy(element, data, arr->item_size);
	arr->length++;
}

void* dynamic_array_at(DynamicArray *arr, size_t index)
{
	return (unsigned char*)arr->data + (index * arr->item_size);
}

void dynamic_array_clear_custom(DynamicArray *arr, void (*custom_free)(void *))
{
	custom_free(arr);
}

void dynamic_array_clear(DynamicArray *arr)
{
	if (arr != NULL) {
		if (arr->data != NULL)
			free(arr->data);
		arr->length = 0;
	}
}

void dynamic_array_free_custom(DynamicArray *arr, void (*custom_free)(void *))
{
	custom_free(arr);
}

void dynamic_array_free(DynamicArray *arr)
{
	if (arr != NULL) {
		dynamic_array_clear(arr);
		free(arr);
		arr = NULL;
	}
}
