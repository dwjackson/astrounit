#include "astrounit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DEFAULT_BUFSIZE 10
#define FAIL_FMT

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
    char *message;

    num_tests = 0;
    num_failures = 0;
    for (i = 0; i < suite->length; i++) {
        test = &(suite->tests)[i];
        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "ERROR: Could not fork()\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            /* In the child, run the test and exit */
            message = (test->run)(test->args);
            if (message == NULL) {
                exit(EXIT_SUCCESS);
            } else {
                exit(EXIT_FAILURE);
            }
        } else {
            /* In the parent, wait for the child to finish the test */
            int status;
            waitpid(pid, &status, 0);
            if (status != 0) {
                printf("Test exited with abornmal exit status: %d\n", status);
                num_failures++;
            }
        }
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

int
astro_suite_num_failures(struct astro_suite *suite)
{
    return suite->num_failures;
}

void
astro_print_fail_int(int expected,
                     int actual,
                     const char *failure_message,
                     const char *file,
                     int line)
{
    char fmt[] = "%s:%d - %s; expected %d, was %d\n";
    printf(fmt, file, line, failure_message, expected, actual);
}

void
astro_print_fail_str(const char *expected,
                     const char *actual,
                     const char *failure_message,
                     const char *file,
                     int line)
{
    char fmt[] = "%s:%d - %s; expected \"%s\", was \"%s\"\n";
    printf(fmt, file, line, failure_message, expected, actual);
}
