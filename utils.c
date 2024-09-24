#include <stdlib.h>
#include <string.h>

#include "utils.h"

/*+----------------------------- DYNAMIC ARRAY ------------------------+*/
void array_init(Array *arr, size_t item_size, size_t capacity)
{
	arr->length = 0;
	arr->capacity = capacity;
	arr->item_size = item_size;
	arr->data = malloc(item_size * capacity);
}

void array_append(Array *arr, void *data)
{
	if (arr->length >= arr->capacity) {
		arr->capacity *= 2;
		arr->data = realloc(arr->data, arr->item_size * arr->capacity);
	}
	unsigned char *element = (unsigned char*)arr->data + (arr->length * arr->item_size);
	memcpy(element, data, arr->item_size);
	arr->length++;
}

void *array_at(Array *arr, size_t index)
{
	return (unsigned char*)arr->data + (index * arr->item_size);
}

void array_clear_custom(Array *arr, void (*custom_destroy)(void *))
{
	if (custom_destroy != NULL) {
		for (size_t i = 0; i < arr->length; i++) {
			void *element = (unsigned char*)arr->data + (i * arr->item_size);
			custom_destroy(element);
		}
	} else {
		if (arr->data != NULL)
			free(arr->data);
	}
	arr->length = 0;
}

void array_clear(Array *arr)
{
	array_clear_custom(arr, NULL);
}

void array_destroy_custom(Array *arr, void (*custom_destroy)(void *))
{
	array_clear_custom(arr, custom_destroy);
	free(arr);
	arr = NULL;
}

void array_destroy(Array *arr)
{
	array_destroy_custom(arr, NULL);
}

