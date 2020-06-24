#include <stdio.h>
#include <stdlib.h>

#define BOILERPLATE "#include <astrounit.h>\n" \
	"#include <stdlib.h>\n\n" \
	"ASTRO_TEST_BEGIN(test_example)\n" \
	"{\n" \
	"\tassert_int_eq(2, 1 + 1, \"Your ALU seems broken!\");\n" \
	"}\n" \
	"ASTRO_TEST_END\n\n" \
	"void add_tests(struct astro_suite *suite)\n" \
	"{\n" \
	"\tastro_suite_add_test(suite, test_example, NULL);\n" \
	"}\n\n" \
	"int main(int argc, char *argv[])\n" \
	"{\n" \
	"\treturn astro_main(argc, argv, add_tests);\n" \
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
