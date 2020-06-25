#include <astrounit.h>
#include <stdlib.h>

ASTRO_TEST_IGNORE(test_example)
{
	assert_int_eq(0, 1, "This fails on purpose");
}
ASTRO_TEST_END

void add_tests(struct astro_suite *suite)
{
	astro_suite_add_test(suite, test_example, NULL);
}

int main(int argc, char *argv[])
{
	return astro_main(argc, argv, add_tests);
}
