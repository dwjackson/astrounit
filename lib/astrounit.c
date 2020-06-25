#include "astrounit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdarg.h>

#define DEFAULT_BUFSIZE 10
#define MAX_TEST_SECONDS 5

#define PASSED '.'
#define FAILED 'x'

#define FLAG_VERBOSE 0x1

/*
 * This is the file to which all of the messages are logged and printed at the
 * end of the execution of the test suite.
 */
FILE *log_file = NULL;

/* Failure Jump Environment
 *
 * This is the environment to which any failed unit test jumps on failure. This
 * is roughly analogous to exception catching.
 */
jmp_buf astro_fail;

struct astro_meta {
	unsigned int flags;
};

struct astro_test {
	astro_ret_t (*run)(struct astro_meta*, void*);
	void *args;
};

struct astro_suite {
	int num_failures;
	unsigned int flags;
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

static int
astro_printf(const char *fmt, ...);

struct astro_suite
*astro_suite_create()
{
	struct astro_suite *suite;
	size_t size = sizeof(struct astro_suite)
		+ DEFAULT_BUFSIZE * sizeof(struct astro_test);
	suite = malloc(size);
	if (suite != NULL) {
		memset(suite, 0, size);
		suite->flags = 0x0;
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
	astro_ret_t (*test_run)(struct astro_meta*, void*),
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
	int fail;
	char status;
	pid_t test_pid;
	pid_t timer_pid;
	int is_verbose = suite->flags & FLAG_VERBOSE;
	char log_file_name[] = "astro_log_XXXXXX";
	int log_fd;
	int ch;

	log_fd = mkstemp(log_file_name);
	if (log_fd == -1) {
		perror("mkstemp");
		return 1;
	}
	log_file = fdopen(log_fd, "w+");
	if (log_file == NULL) {
		perror("fdopen");
		return 1;
	}

	num_tests = 0;
	num_failures = 0;
	for (i = 0; i < suite->length; i++) {
		test = &(suite->tests)[i];
		fail = perform_test(suite, test);
		if (fail) {
			status = FAILED;
		} else {
			status = PASSED;
		}
		if (is_verbose) {
			printf(" %s\n", status == PASSED ? "PASS" : "FAIL");
		} else {
			printf("%c", status);
			fflush(stdout);
		}
		num_failures += fail;
		num_tests++;
	}
	suite->num_failures = num_failures;
	if (!is_verbose) {
		printf("\n");
	}

	/* Print all of the messages that we saved up during the run */
	if (fseek(log_file, 0, SEEK_SET) == -1) {
		perror("fseek");
		fclose(log_file);
		remove(log_file_name);
		return suite->num_failures + 1;
	}
	while ((ch = fgetc(log_file)) != EOF) {
		printf("%c", ch);
	}
	printf("\n");
	fclose(log_file);
	remove(log_file_name);

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
	struct astro_meta meta;

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

		meta.flags = suite->flags;
		retval = (test->run)(&meta, test->args);

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
		failure = 1;
	} else if (pid == timer_pid) {
		astro_printf("Test took too long to finish\n");
		failure = 1;
		kill(test_pid, SIGKILL);
		waitpid(test_pid, &status, 0);
	} else {
		kill(timer_pid, SIGKILL);
		waitpid(timer_pid, &status, 0);
	}
	return failure;
}

static int
astro_printf(const char *fmt, ...)
{
	va_list ap;
	int chars_printed;
	va_start(ap, fmt);
	return vfprintf(log_file, fmt, ap);
}

void
astro_print_fail(const char *failure_message, const char *file, int line)
{
	astro_printf("%s:%d - %s\n", file, line, failure_message);
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
	astro_printf(fmt, file, line, failure_message, expected, actual);
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
	astro_printf(fmt, file, line, failure_message, expected, actual);
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

int
astro_main(int argc, char *argv[], void (*add_tests)(struct astro_suite *s))
{
	int opt;
	int vflag = 0;
	while ((opt = getopt(argc, argv, "v")) != -1) {
		switch (opt) {
			case 'v':
				vflag = 1;
				break;
			default:
				break;
		}
	}

	int num_failures = 0;
	struct astro_suite *suite = astro_suite_create();
	if (vflag) {
		suite->flags |= FLAG_VERBOSE;
	}
	add_tests(suite);
	num_failures = astro_suite_run(suite);
	astro_suite_destroy(suite);
	return num_failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int
astro_is_verbose(struct astro_meta *meta)
{
	return meta->flags & FLAG_VERBOSE;
}
