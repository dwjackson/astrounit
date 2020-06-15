#include <astrounit.h>
#include <stdlib.h>

static void setup(void *args);
static void teardown(void *args);

ASTRO_TEST_BEGIN(test_teardown)
{
	int *arg = astro_test_args(int*);
	assert_int_eq(456, *arg, "Setup didn't run");
}
ASTRO_TEST_END

int main()
{
	int num = 123;

	int num_failures = 0;
	struct astro_suite *suite = astro_suite_create();
	astro_suite_setup(suite, setup, &num);
	astro_suite_teardown(suite, teardown);
	astro_suite_add_test(suite, test_teardown, &num);
	num_failures = astro_suite_run(suite);
	astro_suite_destroy(suite);
	return num_failures == 1 ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void setup(void *args)
{
	int *num = (int *)args;
	*num = 456;
}

static void teardown(void *args)
{
	int *num = (int *)args;
	assert_int_eq(123, *num, "teardown should fail");
}
