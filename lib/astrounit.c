#include "astrounit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define DEFAULT_BUFSIZE 10
#define MAX_TEST_TIME 5

struct astro_test {
	astro_ret_t (*run)(void*);
	void *args;
};

struct astro_suite {
	int num_failures;
	size_t bufsize;
	size_t length;
	struct astro_test tests[];
};

static int 
perform_test(struct astro_test *test);

static void
run_timer();

static int 
wait_for_test(pid_t test_pid, pid_t timer_pid);

struct astro_suite
*astro_suite_create()
{
	struct astro_suite *suite;
	size_t size = sizeof(struct astro_suite)
		+ DEFAULT_BUFSIZE * sizeof(struct astro_test);
	suite = malloc(size);
	if (suite != NULL) {
		memset(suite, 0, size);
		suite->bufsize = DEFAULT_BUFSIZE;
	}
	return suite;
}

void
astro_suite_destroy(struct astro_suite *suite)
{
	free(suite);
}

void
astro_suite_add_test(struct astro_suite *suite,
	astro_ret_t (*test_run)(void*),
	void *args)
{
	if (suite->length + 1 >= suite->bufsize) {
		suite->bufsize *= 2;
		size_t size = sizeof(struct astro_suite)
			+ sizeof(struct astro_test) * suite->bufsize;
		suite = realloc(suite, size);
	}
	struct astro_test test;
	test.run = test_run;
	test.args = args;
	(suite->tests)[suite->length] = test;
	suite->length++;
}

void
astro_suite_run(struct astro_suite *suite)
{
	int i;
	struct astro_test *test;
	int num_tests;
	int num_failures;
	pid_t test_pid;
	pid_t timer_pid;

	num_tests = 0;
	num_failures = 0;
	for (i = 0; i < suite->length; i++) {
		test = &(suite->tests)[i];
		num_failures += perform_test(test);
		num_tests++;
	}
	suite->num_failures = num_failures;
	if (num_failures == 0) {
		printf("----------------------------------------\n");
		printf(" ALL TESTS PASSED\n");
		printf("----------------------------------------\n");
	} else {
		printf("----------------------------------------\n");
		printf(" %d FAILURES / %d TESTS\n", num_failures, num_tests);
		printf("----------------------------------------\n");
	}
}

static int 
perform_test(struct astro_test *test)
{
	pid_t test_pid;
	pid_t timer_pid;
	astro_ret_t retval;
	int failure = 0;

	test_pid = fork();
	if (test_pid < 0) {
		fprintf(stderr, "ERROR: Could not fork()\n");
		exit(EXIT_FAILURE);
	} else if (test_pid == 0) {
		/* In the child, run the test and exit */
		retval = (test->run)(test->args);
		if (retval == 0) {
			exit(EXIT_SUCCESS);
		} else {
			exit(EXIT_FAILURE);
		}
	} else {
		/* In the parent */
		timer_pid = fork();
		if (timer_pid < 0) {
			fprintf(stderr, "ERROR: Could not fork()\n");
			exit(EXIT_FAILURE);
		} else if (timer_pid == 0) {
			run_timer();
		} else {
			wait_for_test(test_pid, timer_pid);
		}
	}

	return failure;
}

/*
 * Run a timer to catch infinite loops
 */
static void
run_timer()
{
	clock_t c;
	do {
		c = clock();
	} while (c / CLOCKS_PER_SEC < MAX_TEST_TIME);
	exit(EXIT_SUCCESS);
}

/*
 * Wait for the test or timer to finish
 */
static int 
wait_for_test(pid_t test_pid, pid_t timer_pid)
{
	pid_t pid;
	int status;
	int failure = 0;

	pid = wait(&status);
	if (pid == test_pid && status != 0) {
		printf("Test exited with abornmal exit status: %d\n", status);
		failure = 1;
	} else if (pid == timer_pid) {
		printf("Test took too long to finish\n");
		failure = 1;
		kill(test_pid, SIGKILL);
		waitpid(test_pid, &status, 0);
	} else {
		kill(timer_pid, SIGKILL);
		waitpid(timer_pid, &status, 0);
	}
	return failure;
}

int
astro_suite_num_failures(struct astro_suite *suite)
{
	return suite->num_failures;
}

void
astro_print_fail_int(
	int expected,
	int actual,
	const char *failure_message,
	const char *file,
	int line
)
{
	char fmt[] = "%s:%d - %s; expected %d, was %d\n";
	printf(fmt, file, line, failure_message, expected, actual);
}

void
astro_print_fail_str(
	const char *expected,
	const char *actual,
	const char *failure_message,
	const char *file,
	int line
)
{
	char fmt[] = "%s:%d - %s; expected \"%s\", was \"%s\"\n";
	printf(fmt, file, line, failure_message, expected, actual);
}
