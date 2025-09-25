#include "jstring.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define MAX_LOGGER_MESSAGE_SIZE 1024

void print_memory_ascii(char *address, u64 offset, u64 size)
{
	u64 index;
	char *tmp = ((char*)jstring_temporary_memory_info.address) + 
		offset;

	printf("print_memory: %p + %llu (%llu bytes):\n",
			address, offset, size);
	for(index = 0; index < size; index++)
	{
		if(index % 50 == 0 && index != 0)
		{
			putchar('\n');
		}
		else if(tmp[index] == '\0')
		{
			putchar('|');
		}
		else if(tmp[index] == '\n')
		{
			putchar('\\');
			putchar('n');
		}
		else if(tmp[index] == '\r')
		{
			putchar('\\');
			putchar('r');
		}
		else if(tmp[index] == '\t')
		{
			putchar('\\');
			putchar('t');
		}
		else 
		{
			putchar(tmp[index]);
		}
	}
	putchar('\n');
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
	void *string_memory = malloc(4096);
	memset(string_memory, '-', 4096);
	jstring_load_logging_function(jstring_logger);
	jstring_memory_activate(4096, string_memory);

	jstring trim_left = jstring_create_temporary(
			"\n\t trim_left \n\t",
			jstring_length("\n\t trim_left \n\t"));
	jstring trim_right = jstring_create_temporary(
			"\n\t trim_right \n\t",
			jstring_length("\n\t trim_right \n\t"));
	jstring trim = jstring_create_temporary(
			"\n\t trim \n\t",
			jstring_length("\n\t trim \n\t"));
	print_memory_ascii(string_memory, 0, 300);

	if(!jstring_trim_left(&trim_left))
	{
		printf("WOMP WOMP\n");
		return -1;
	}
	print_memory_ascii(string_memory, 0, 300);
	if(!jstring_trim_right(&trim_right))
	{
		printf("WOMP WOMP\n");
		return -1;
	}
	print_memory_ascii(string_memory, 0, 300);
	if(!jstring_trim(&trim))
	{
		printf("WOMP WOMP\n");
		return -1;
	}
	print_memory_ascii(string_memory, 0, 300);

	printf("trim_left: %s\n", trim_left.data);
	printf("trim_right: %s\n", trim_right.data);
	printf("trim: %s\n", trim.data);

	return(0);
}
