#ifndef ATROUNIT_H
#define ATROUNIT_H

#include <string.h>
#include <setjmp.h>
#include <stdio.h>

#define ASTRO_IGNORE -2
#define ASTRO_FAIL -1
#define ASTRO_PASS 0

typedef int astro_ret_t;

#define ASTRO_TEST_META _meta
#define ASTRO_TEST_ARGS_NAME _args

#define ASTRO_TEST_BEGIN(test_name)   \
astro_ret_t test_name(struct astro_meta *ASTRO_TEST_META, void *ASTRO_TEST_ARGS_NAME) \
{ \
	if (astro_is_verbose(ASTRO_TEST_META)) { \
		printf("%s", __func__); \
	} \
	extern jmp_buf astro_fail; \
	if (setjmp(astro_fail) == 0) { \
		do \

#define ASTRO_TEST_IGNORE(test_name)   \
astro_ret_t test_name(struct astro_meta *ASTRO_TEST_META, void *ASTRO_TEST_ARGS_NAME) \
{ \
	if (astro_is_verbose(ASTRO_TEST_META)) { \
		printf("%s", __func__); \
	} \
	return ASTRO_IGNORE; \
	extern jmp_buf astro_fail; \
	if (setjmp(astro_fail) == 0) { \
		do \

#define ASTRO_TEST_END \
while (0);     \
} else { \
	return ASTRO_FAIL; \
} \
	return ASTRO_PASS; \
}

#define astro_test_args(type) ((type) ASTRO_TEST_ARGS_NAME)

struct astro_meta;

struct astro_suite;

struct astro_suite
*astro_suite_create();

void
astro_suite_destroy(struct astro_suite *suite);

void
astro_suite_add_test(struct astro_suite *suite,
                     astro_ret_t (*test)(struct astro_meta*, void*),
                     void *args);

int
astro_suite_run(struct astro_suite *suite);

void
astro_suite_setup(struct astro_suite *suite, void (*setup)(void*), void *args);

void
astro_suite_teardown(struct astro_suite *suite, void (*teardown)(void*));

int
astro_main(int argc, char *argv[], void (*add_tests)(struct astro_suite *s));

int
astro_is_verbose(struct astro_meta *meta);


/****************************************************************************
 * ERROR PRINTERS                                                           *
 ****************************************************************************/

void
astro_print_fail(const char *failure_message, const char *file, int line);

void
astro_print_fail_int(int expected,
                     int actual,
                     const char *failure_message,
                     const char *file,
                     int line);

void
astro_print_fail_str(const char *expected,
                     const char *actual,
                     const char *failure_message,
                     const char *file,
                     int line);

/*****************************************************************************
 * ASSERTS                                                                   *
 *****************************************************************************/

/* Unconditional failure */
#define fail(failure_message) do { \
	extern jmp_buf astro_fail; \
	astro_print_fail(failure_message, __FILE__, __LINE__); \
        longjmp(astro_fail, 0); \
} while (0)

/* Generic assert */
#define assert(test, failure_message) do { \
    if (!(test)) {                         \
        extern jmp_buf astro_fail;         \
	astro_print_fail(failure_message, __FILE__, __LINE__); \
        longjmp(astro_fail, 0);            \
    }                                      \
} while (0)

/* Assert that two integers are equal */
#define assert_int_eq(expected, actual, msg)                                 \
    do {                                                                     \
        if ((expected) != (actual)) {                                        \
            extern jmp_buf astro_fail;                                       \
            astro_print_fail_int(expected, actual, msg, __FILE__, __LINE__); \
            longjmp(astro_fail, 0);                                          \
        }                                                                    \
    } while (0)

/* Assert that two doubles are (roughly) equal */
#define assert_double_eq(expected, actual, threshold, msg)                   \
    do {                                                                     \
        double lower = expected - threshold;                                 \
        double upper = expected + threshold;                                 \
        if (actual < lower || actual > upper) {                              \
            extern jmp_buf astro_fail;                                       \
            astro_print_fail_int(expected, actual, msg, __FILE__, __LINE__); \
            longjmp(astro_fail, 0);                                          \
        }                                                                    \
    } while (0)

/* Assert that two strings are equal */
#define assert_str_eq(expected, actual, msg)                                 \
    do {                                                                     \
        if (strcmp((expected), (actual)) != 0) {                             \
            extern jmp_buf astro_fail;                                       \
            astro_print_fail_str(expected, actual, msg, __FILE__, __LINE__); \
            longjmp(astro_fail, 0);                                          \
        }                                                                    \
    } while (0)

#endif /* ATROUNIT_H */
