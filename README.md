# Astronomic Unit - A unit testing framework for C

AstroUnit is a simple unit testing framework for C. AstroUnit works by letting
you create small executables that execute tests. If the executable returns a
0 from its main routine, the test passed. Any other return value indicates
that it failed. This allows it to work with various build systems.

## Using AstroUnit with Autotools

If you use the Autotools for building your project, it's fairly easy to
integrate AstroUnit tests. To implement the `make check` target, first add a
directory in which to keep your tests (e.g. a `tests` directory). In that
directory, unit tests are defined in `.c` files. These must be linked with
`libastrounit` in order to actually run the tests.

### Example

We have the following function we'd like to test:

```c
int absolute_value(int x)
{
    return x; // This function is deliberately wrong
}
```

So we create a test file called `test_absolute_value.c` which contains some
necessary setup boilerplate. We define a test for `absolute_value()` as well.

```c
#include "absolute_value.h"

ASTRO_TEST_BEGIN(test_absolute_value)
{
    int x = -2;
    int abs_x = 2;
    int value_to_test = absolute_value(x);
    assert_int_eq(abs_x, value_to_test, "Absolute value of -2 should be 2");
}
ASTRO_TEST_END

int 
main(void)
{
    int num_failures;
    struct astro_suite *suite;

    suite = astro_suite_create();
    astro_suite_add_test(suite, test_absolute_value, NULL);
    astro_suite_run(suite);
    num_failures = astro_suite_num_failures(suite);
    astro_suite_destroy(suite);

    return (num_failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
```

Now we add this test to the `Makefile.am` file:

```m4
TESTS = test_absolute_value

bin_PROGRAMS = test_absolute_value

test_absolute_value_SOURCES = test_absolute_value.c
test_absolute_value_CFLAGS = -g -Wall
```

And now we simply run `make check` to see if the test passes. As you have no
doubt noticed, it will fail because `absolute_value()` is incorrect. We will
change it to the following to make it work:

```c
int absolute_value(int x)
{
    return (x >= 0) ? x : (-1 * x);
}
```

Now if we run `make check` the tests all pass.

## License

AstroUnit is licensed under the MIT license.
