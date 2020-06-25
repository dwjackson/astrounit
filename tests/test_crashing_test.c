#include <astrounit.h>
#include <stdlib.h>

ASTRO_TEST_BEGIN(test_example)
{
	abort(); /* Crash */
}
ASTRO_TEST_END

void add_tests(struct astro_suite *suite)
{
	astro_suite_add_test(suite, test_example, NULL);
}

int main(int argc, char *argv[])
{
	int ret = astro_main(argc, argv, add_tests);
	return ret == 1 ? EXIT_SUCCESS : EXIT_FAILURE;
}
