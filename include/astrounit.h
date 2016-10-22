#ifndef ATROUNIT_H
#define ATROUNIT_H

#include <string.h>

typedef char *astro_ret_t;

#define ASTRO_TEST_BEGIN(test_name)   \
    astro_ret_t test_name(void *args) \
    {                                 \
        do

#define ASTRO_TEST_END \
        while (0);     \
        return 0;      \
    }

struct astro_suite;

struct astro_suite
*astro_suite_create();

void
astro_suite_destroy(struct astro_suite *suite);

void
astro_suite_add_test(struct astro_suite *suite,
                     astro_ret_t (*test)(void*),
                     void *args);

void
astro_suite_run(struct astro_suite *suite);

int
astro_suite_num_failures(struct astro_suite *suite);


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
    if (!(test)) {                           \
        printf("%s\n", failure_message);     \
        return failure_message;              \
    }                                        \
} while (0)

/* Assert that two integers are equal */
#define assert_int_eq(expected, actual, msg)                               \
    do {                                                                     \
        if ((expected) != (actual)) {                                        \
            astro_print_fail_int(expected, actual, msg, __FILE__, __LINE__); \
            return msg;                                                      \
        }                                                                    \
    } while (0)

/* Assert that two strings are equal */
#define assert_str_eq(expected, actual, msg)                               \
    do {                                                                     \
        if (strcmp((expected), (actual)) != 0) {                             \
            astro_print_fail_str(expected, actual, msg, __FILE__, __LINE__); \
            return msg;                                                      \
        }                                                                    \
    } while (0)

#endif /* ATROUNIT_H */
