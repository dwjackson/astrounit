#include "astrounit.h"
#include <stdlib.h>

ASTRO_TEST_BEGIN(test_str_eq)
{
	char str1[] = "test";
	char str2[] = "test";
	assert_str_eq(str1, str2, "assert_str_eq() is wrong");
}
ASTRO_TEST_END

struct test_args {
	int a;
};

ASTRO_TEST_BEGIN(test_arguments)
{
	struct test_args *arguments = astro_test_args(struct test_args *);
	assert_int_eq(123, arguments->a, "Wrong argument: a");
}
ASTRO_TEST_END

ASTRO_TEST_BEGIN(test_double_equals)
{
	assert_double_eq(123.45, 123.46, 0.01, "doubles not equal");
	assert_double_eq(123.45, 123.45, 0.01, "doubles not equal");
	assert_double_eq(123.45, 123.44, 0.01, "doubles not equal");
}
ASTRO_TEST_END

int
main(void)
{
	int num_failures;
	struct astro_suite *suite;

	suite = astro_suite_create();

	astro_suite_add_test(suite, test_str_eq, NULL);

	struct test_args args = { 123 };
	astro_suite_add_test(suite, test_arguments, &args);
	astro_suite_add_test(suite, test_double_equals, NULL);

	num_failures = astro_suite_run(suite);
	astro_suite_destroy(suite);

	return (num_failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
