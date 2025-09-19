#include "jstring.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define MAX_LOGGER_MESSAGE_SIZE 1024

void print_memory(char *address, u64 offset, u64 size)
{
	u64 index;
	char *tmp = ((char*)jstring_temporary_memory_info.address) + 
		offset;

	printf("print_memory: %p + %llu (%llu bytes):\n",
			address, offset, size);
	for(index = 0; index < size; index++)
	{
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
	void *string_memory = malloc(1024);
	memset(string_memory, '-', 1024);
	jstring_load_logging_function(jstring_logger);
	jstring_memory_activate(1024, string_memory);

	jstring first = jstring_create_temporary("first", 5);
	jstring second = jstring_create_temporary("second", 6);
	jstring_memory_reset(1024, string_memory);
	memset(string_memory, '-', 1024);
	jstring third = jstring_create_temporary("third", 5);
	jstring fourth = jstring_create_temporary("fourth", 6);

	print_memory(string_memory, 0, 1024);

	return(0);
}
