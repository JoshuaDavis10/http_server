#include "jstring.h"

#include <stdio.h>

#include "jstring_regression_tests.c"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define MAX_LOGGER_MESSAGE_SIZE 1024

#define MAX_TESTS 100
#define TEST_NAME_LENGTH 100

typedef b32 (*test_function)();

typedef struct
{
	char name[TEST_NAME_LENGTH];
	test_function t;
} test;

typedef struct 
{
	u32 passed;
	u32 failed;
	u32 num_tests;
	test tests[MAX_TESTS];
} test_info;

static test_info ti;

void run_tests()
{
	ti.passed = 0;
	ti.failed = 0;

	u32 index;
	b32 result;
	for(index = 0; index < ti.num_tests; index++)
	{
		printf("\n\e[1;37mRUNNING TEST: \e[1;35m%s...\n\e[0;37m",
				ti.tests[index].name);
		result = ti.tests[index].t();
		if(result == true)
		{
			ti.passed++;
			printf("\e[1;37mRESULT: \e[1;35m%s\e[1;32m PASSED.\e[0m\n", 
					ti.tests[index].name);
		}
		else if(result == false)
		{
			ti.passed++;
			printf("\e[1;37mRESULT: \e[1;35m%s\e[1;31m FAILED!\e[0m\n", 
					ti.tests[index].name);
		}
		else 
		{
			JSTRING_ASSERT(0, " ");
		}
	}
	printf("\n\e[1;37mTEST RESULTS: \e[1;32m%u\e[0;37m passed, "
			"\e[1;31m%u\e[0;37m failed.\n\n\n\n",  
			ti.passed, ti.failed);
}

void add_test(test_function t, const char *name, u32 name_length)
{
	if(name_length > TEST_NAME_LENGTH)
	{
		JSTRING_ASSERT(0, " ");
	}
	if(ti.num_tests < MAX_TESTS)
	{
		test new_test;
		new_test.t = t;
		u32 index;
		for(index = 0; index < name_length; index++)
		{
			new_test.name[index] = name[index];
		}
		new_test.name[index] = '\0';
		ti.tests[ti.num_tests] = new_test;
		ti.num_tests++;
	}
	else 
	{
		JSTRING_ASSERT(0, " ");
	}
}

void print_memory_ascii(char *address, u64 offset, u64 size)
{
	u64 index;
	char *tmp = ((char*)jstring_temporary_memory_info.address) + 
		offset;

	printf("print_memory: %p + %llu (%llu bytes):\n",
			address, offset, size);
	for(index = 0; index < size; index++)
	{
		if(index % 50 == 0)
		{
			putchar('\n');
		}
		if(tmp[index] == '\0')
		{
			putchar('|');
		}
		putchar(tmp[index]);
	}
	putchar('\n');
}

void jstring_logger(const char *message, ...) {
	char output[MAX_LOGGER_MESSAGE_SIZE];
	va_list arg_ptr;
	va_start(arg_ptr, message);
    vsnprintf(output, MAX_LOGGER_MESSAGE_SIZE, message, arg_ptr);
	va_end(arg_ptr);
	printf("\e[1;36m[JSTRING]:\e[0;36m %s\e[0m\n", output);
}

int main()
{
	printf("\n\n\n\e[1;37mJSTRING REGRESSION TEST PROGRAM\n\e[0;37m\n");

	void *string_memory = malloc(1024);
	memset(string_memory, '-', 1024);
	jstring_load_logging_function(jstring_logger);
	jstring_memory_activate(1024, string_memory);

	add_test(jstring_create_temporary_test, 
			"jstring create temporary test",
			jstring_length("jstring_create_temporary_test"));
	run_tests();

	return(0);
}
