#include "astrounit.h"
#include <stdlib.h>

ASTRO_TEST_BEGIN(test_str_eq)
{
	char str1[] = "test";
	char str2[] = "test2";
	assert_str_eq(str1, str2, "strings are equal (they should not be)");
}
ASTRO_TEST_END

static void fails()
{
	char str1[] = "func";
	char str2[] = "func2";
	assert_str_eq(str1, str2, "strings are equal (they should not be)");
}

ASTRO_TEST_BEGIN(test_fail_in_function)
{
	fails();
}
ASTRO_TEST_END

int
main(void)
{
	int num_failures;
	struct astro_suite *suite;

	suite = astro_suite_create();
	astro_suite_add_test(suite, test_str_eq, NULL);
	astro_suite_add_test(suite, test_fail_in_function, NULL);
	astro_suite_run(suite);
	num_failures = astro_suite_num_failures(suite);
	astro_suite_destroy(suite);

	return (num_failures == 1 ? EXIT_SUCCESS : EXIT_FAILURE);
}
