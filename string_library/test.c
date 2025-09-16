#include "jstring.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_LOGGER_MESSAGE_SIZE 1024

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
	jstring_load_logging_function(jstring_logger);
	jstring_memory_activate(1024, string_memory);
	return(0);
}
