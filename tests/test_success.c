#include "astrounit.h"
#include <stdlib.h>

ASTRO_TEST_BEGIN(test_str_eq)
{
	char str1[] = "test";
	char str2[] = "test";
	assert_str_eq(str1, str2, "assert_str_eq() is wrong");
}
ASTRO_TEST_END

int
main(void)
{
	int num_failures;
	struct astro_suite *suite;

	suite = astro_suite_create();
	astro_suite_add_test(suite, test_str_eq, NULL);
	astro_suite_run(suite);
	num_failures = astro_suite_num_failures(suite);
	astro_suite_destroy();

	return (num_failues == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
