TESTS = test_success test_failure test_setup

check_PROGRAMS = test_success test_failure test_setup

test_success_SOURCES = test_success.c
test_success_CFLAGS = -g -Wall -I$(top_builddir)/include
test_success_LDADD = $(top_builddir)/lib/libastrounit.la

test_failure_SOURCES = test_failure.c
test_failure_CFLAGS = -g -Wall -I$(top_builddir)/include
test_failure_LDADD = $(top_builddir)/lib/libastrounit.la

test_setup_SOURCES = test_setup.c
test_setup_CFLAGS = -g -Wall -I$(top_builddir)/include
test_setup_LDADD = $(top_builddir)/lib/libastrounit.la
