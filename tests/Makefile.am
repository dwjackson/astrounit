TESTS = test_success test_failure test_setup test_teardown test_astro_main \
	test_ignore test_crashing_test

check_PROGRAMS = test_success test_failure test_setup test_teardown \
		 test_astro_main test_ignore test_crashing_test

test_success_SOURCES = test_success.c
test_success_CFLAGS = -g -Wall -I$(top_builddir)/include
test_success_LDADD = $(top_builddir)/lib/libastrounit.la

test_failure_SOURCES = test_failure.c
test_failure_CFLAGS = -g -Wall -I$(top_builddir)/include
test_failure_LDADD = $(top_builddir)/lib/libastrounit.la

test_setup_SOURCES = test_setup.c
test_setup_CFLAGS = -g -Wall -I$(top_builddir)/include
test_setup_LDADD = $(top_builddir)/lib/libastrounit.la

test_teardown_SOURCES = test_teardown.c
test_teardown_CFLAGS = -g -Wall -I$(top_builddir)/include
test_teardown_LDADD = $(top_builddir)/lib/libastrounit.la

test_astro_main_SOURCES = test_astro_main.c
test_astro_main_CFLAGS = -g -Wall -I$(top_builddir)/include
test_astro_main_LDADD = $(top_builddir)/lib/libastrounit.la

test_ignore_SOURCES = test_ignore.c
test_ignore_CFLAGS = -g -Wall -I$(top_builddir)/include
test_ignore_LDADD = $(top_builddir)/lib/libastrounit.la

test_crashing_test_SOURCES = test_crashing_test.c
test_crashing_test_CFLAGS = -g -Wall -I$(top_builddir)/include
test_crashing_test_LDADD = $(top_builddir)/lib/libastrounit.la
