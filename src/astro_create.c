#include <stdio.h>
#include <stdlib.h>

#define BOILERPLATE "#include <astrounit.h>\n" \
	"#include <stdlib.h>\n\n" \
	"ASTRO_TEST_BEGIN(test_example)\n" \
	"{\n" \
	"\tassert_int_eq(2, 1 + 1, \"Your ALU seems broken!\");\n" \
	"}\n" \
	"ASTRO_TEST_END\n\n" \
	"int main()\n" \
	"{\n" \
	"\tint num_failures = 0;\n" \
	"\tstruct astro_suite *suite = astro_suite_create();\n" \
	"\tastro_suite_add_test(suite, test_example, NULL);\n" \
	"\tnum_failures = astro_suite_run(suite);\n" \
	"\tastro_suite_destroy(suite);\n" \
	"\treturn num_failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;\n" \
	"}\n"

int main(int argc, char *argv[])
{
	char *file_name;
	FILE *fp;

	if (argc < 2) {
		fp = stdout;
	} else {
		file_name = argv[1];
		fp = fopen(file_name, "w");
		if (fp == NULL) {
			perror("fopen");
			exit(EXIT_FAILURE);
		}
	}
	fprintf(fp, "%s", BOILERPLATE);
	fclose(fp);

	return 0;
}
