#include "astrounit.h"
#include <stdlib.h>

/*
 * If you add/remove a test that should fail, change this number:
 */
#define EXPECTED_FAIL_COUNT 4

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

ASTRO_TEST_BEGIN(test_double_equals)
{
	assert_double_eq(123.45, 123.4, 0.01, "doubles not equal");
}
ASTRO_TEST_END

ASTRO_TEST_BEGIN(test_unconditional_failure)
{
	fail("Unconditional failure test");
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
	astro_suite_add_test(suite, test_double_equals, NULL);
	astro_suite_add_test(suite, test_unconditional_failure, NULL);
	num_failures = astro_suite_run(suite);
	astro_suite_destroy(suite);

	if (num_failures == EXPECTED_FAIL_COUNT) {
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}
