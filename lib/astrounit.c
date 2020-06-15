#include "astrounit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define DEFAULT_BUFSIZE 10
#define MAX_TEST_SECONDS 5

/* Failure Jump Environment
 *
 * This is the environment to which any failed unit test jumps on failure. This
 * is roughly analogous to exception catching.
 */
jmp_buf astro_fail;

struct astro_test {
	astro_ret_t (*run)(void*);
	void *args;
};

struct astro_suite {
	int num_failures;
	size_t bufsize;
	size_t length;
	void (*setup)(void*);
	void *setup_args;
	void (*teardown)(void*);
	struct astro_test tests[];
};

static void 
do_nothing(void *args);

static int 
perform_test(struct astro_suite *suite, struct astro_test *test);

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
		suite->setup = do_nothing;
		suite->setup_args = NULL;
		suite->teardown = do_nothing;
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
	size_t size;
	struct astro_test test;

	if (suite->length + 1 >= suite->bufsize) {
		suite->bufsize *= 2;
		size = sizeof(struct astro_suite)
			+ sizeof(struct astro_test) * suite->bufsize;
		suite = realloc(suite, size);
	}
	test.run = test_run;
	test.args = args;
	(suite->tests)[suite->length] = test;
	suite->length++;
}

int
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
		num_failures += perform_test(suite, test);
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
	return suite->num_failures;
}

static int 
perform_test(struct astro_suite *suite, struct astro_test *test)
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

		if (setjmp(astro_fail) == 0) {
			suite->setup(suite->setup_args);
		} else {
			exit(EXIT_FAILURE);
		}

		retval = (test->run)(test->args);

		if (setjmp(astro_fail) == 0) {
			suite->teardown(suite->setup_args);
			if (retval == 0) {
				exit(EXIT_SUCCESS);
			} else {
				exit(EXIT_FAILURE);
			}
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
			failure = wait_for_test(test_pid, timer_pid);
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
	} while (c / CLOCKS_PER_SEC < MAX_TEST_SECONDS);
	exit(EXIT_SUCCESS);
}

/*
 * Wait for the test or timer to finish, then depending on which finishes
 * first, kill the other process and wait for it to finish as well. If the
 * timer finishes first then the test has taken too long to run and fails, but
 * if the test finishes first the timer is no longer needed and is killed.
 */
static int 
wait_for_test(pid_t test_pid, pid_t timer_pid)
{
	pid_t pid;
	int status;
	int failure = 0;

	pid = wait(&status);
	if (pid == test_pid && status != 0) {
		printf("Test exited with abnormal exit status: %d\n", status);
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

void
astro_suite_setup(struct astro_suite *suite, void (*setup)(void*), void *args)
{
	suite->setup = setup;
	suite->setup_args = args;
}

static void 
do_nothing(void *args)
{
	(void)args;
}

void
astro_suite_teardown(struct astro_suite *suite, void (*teardown)(void*))
{
	suite->teardown = teardown;
}
