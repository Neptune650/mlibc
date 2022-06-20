#include <assert.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define dump(x) fprintf(stderr, "getopt c '%c' (%d), optind %d ('%s'), optarg '%s', optopt '%c' (%d)\n", (x), (x), optind, test_argv[optind], optarg, optopt, optopt);

void test1() {
	const char *shortopts = "b:cdef";

	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, no_argument, NULL, 0}
	};

	int test_argc = 7;

	char *test_argv[] = {
		"dummy",
		"--foo",
		"abc",
		"-b",
		"abc",
		"abc",
		"abc"
	};

	optind = 0;
	int c = getopt_long(test_argc, test_argv, shortopts, longopts, NULL);
	dump(c)
	assert(c == 'f');

	c = getopt_long(test_argc, test_argv, shortopts, longopts, NULL);
	dump(c)
	assert(optarg && !strcmp(optarg, "abc"));
	assert(c == 'b');

	c = getopt_long(test_argc, test_argv, shortopts, longopts, NULL);
	dump(c);
	assert(c == -1);
	assert(optarg == NULL);

	// we have 2 non-option arguments
	assert((optind + 2) == test_argc);
}

void test2() {
	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, no_argument, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-c",
	};

	puts("Situation: we pass a non-existant short option in argv");

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:", longopts, NULL);
	// Exprected result: return '?', optopt is set to the offender
	assert(c == '?');
	assert(optopt == 'c');

	puts("Situation: we pass a non-existant short option in argv, while passing a leading colon in optstring");
	optind = 0;

	c = getopt_long(COUNT_OF(test_argv), test_argv, ":ab:", longopts, NULL);
	// Exprected result: return '?', optopt is set to the offender
	assert(c == '?');
	assert(optopt == 'c');

	puts("Situation: we omit the required arg to a short option");
	optind = 0;

	c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:c:", longopts, NULL);
	// Exprected result: return '?', optopt is set to the offender
	assert(c == '?');
	assert(optopt == 'c');

	puts("Situation: we omit the required arg to a short option, while passing a leading colon in optstring");
	optind = 0;

	c = getopt_long(COUNT_OF(test_argv), test_argv, ":ab:c:", longopts, NULL);
	// Exprected result: return ':', optopt is set to the offender
	assert(c == ':');
	assert(optopt == 'c');
}

void test3() {
	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, no_argument, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-cmanagarm",
	};

	puts("Situation: we pass a concatenated argument to a short option");

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:c:", longopts, NULL);
	dump(c);
	// Exprected result:
	assert(c == 'c');
	assert(!strcmp(optarg, "managarm"));
}

void test4() {
	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, no_argument, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-acmanagarm",
	};

	puts("Situation: we pass concatenated short options to getopt");

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:c:", longopts, NULL);
	assert(c == 'a');

	c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:c:", longopts, NULL);
	// Exprected result:
	assert(c == 'c');
	assert(!strcmp(optarg, "managarm"));
}

void test5() {
	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, no_argument, NULL, 0}
	};

	char *test_argv[] = {
		"su",
		"-",
		"managarm",
		0
	};

	int test_argc = 3;

	puts("Situation: testing `su - managarm`");

	optind = 0;
	int c = getopt_long(test_argc, test_argv, "ab:", longopts, NULL);
	dump(c);
	assert(c == -1);

	if (optind < test_argc && !strcmp(test_argv[optind], "-")) {
		++optind;
	}

	assert(optind < test_argc);
	assert(!strcmp(test_argv[optind++], "managarm"));
	assert(optind == test_argc);
}

void test6() {
	char *test_argv[] = {
		"telescope",
		"gemini://electrode.codes",
		"-S",
		0
	};

	int test_argc = 3;
	optind = 0;

	const struct option longopts[] = {
		{"colors",      no_argument,    NULL,   'C'},
		{"colours",	no_argument,    NULL,   'C'},
		{"help",	no_argument,	NULL,	'h'},
		{"safe",	no_argument,	NULL,	'S'},
		{"version",	no_argument,	NULL,	'v'},
		{NULL,		0,		NULL,	0},
	};

	int c = getopt_long(test_argc, test_argv, "Cc:hnST:v", longopts, NULL);
	dump(c);
	assert(c == 'S');
	assert(optind == 3);
	assert(!optarg);
}

int main() {
	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
}
