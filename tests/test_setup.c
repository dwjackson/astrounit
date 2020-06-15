#include <astrounit.h>
#include <stdlib.h>

static void setup(void *args);

ASTRO_TEST_BEGIN(test_setup)
{
	int *arg = astro_test_args(int*);
	assert_int_eq(456, *arg, "Setup didn't set number");
}
ASTRO_TEST_END

int main()
{
	int num = 123;

	int num_failures = 0;
	struct astro_suite *suite = astro_suite_create();
	astro_suite_setup(suite, setup, &num);
	astro_suite_add_test(suite, test_setup, &num);
	num_failures = astro_suite_run(suite);
	astro_suite_destroy(suite);
	return num_failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void setup(void *args)
{
	int *num = (int *)args;
	*num = 456;
}
