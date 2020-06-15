#ifndef ATROUNIT_H
#define ATROUNIT_H

#include <string.h>
#include <setjmp.h>

#define ASTRO_FAIL -1
#define ASTRO_PASS 0

typedef int astro_ret_t;

#define ASTRO_TEST_ARGS_NAME _args

#define ASTRO_TEST_BEGIN(test_name)   \
astro_ret_t test_name(void *ASTRO_TEST_ARGS_NAME) \
{ \
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

struct astro_suite;

struct astro_suite
*astro_suite_create();

void
astro_suite_destroy(struct astro_suite *suite);

void
astro_suite_add_test(struct astro_suite *suite,
                     astro_ret_t (*test)(void*),
                     void *args);

int
astro_suite_run(struct astro_suite *suite);

void
astro_suite_setup(struct astro_suite *suite, void (*setup)(void*), void *args);


/****************************************************************************
 * ERROR PRINTERS                                                           *
 ****************************************************************************/

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

/* Generic assert */
#define assert(test, failure_message) do { \
    if (!(test)) {                         \
        extern jmp_buf astro_fail;         \
        printf("%s\n", failure_message);   \
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
