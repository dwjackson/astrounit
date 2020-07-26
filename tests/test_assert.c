#include <astrounit.h>
#include <stdlib.h>

ASTRO_TEST_BEGIN(test_int_eq)
{
	assert_int_eq(2, 1 + 1, "Your ALU seems broken!");
}
ASTRO_TEST_END

ASTRO_TEST_BEGIN(test_str_eq)
{
	assert_str_eq("abc", "abc", "strings not considered equal");
}
ASTRO_TEST_END

ASTRO_TEST_BEGIN(test_ptr_eq)
{
	int i = 42;
	int *ip = &i;
	assert_ptr_eq(ip, &i, "pointers are not equal");
}
ASTRO_TEST_END

void add_tests(struct astro_suite *suite)
{
	astro_suite_add_test(suite, test_int_eq, NULL);
	astro_suite_add_test(suite, test_str_eq, NULL);
	astro_suite_add_test(suite, test_ptr_eq, NULL);
}

int main(int argc, char *argv[])
{
	return astro_main(argc, argv, add_tests);
}
