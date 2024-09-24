#include "test.h"

#include <unistd.h>

int main(void)
{
	test_str_cstr_to_str();
	/*test_str_array();*/
	test_str_normalize();
	test_str_builder();
	test_str_merge();
	test_str_concatenate();
}
