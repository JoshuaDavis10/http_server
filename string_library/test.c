#include "jstring.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_LOGGER_MESSAGE_SIZE 1024

void LOG_WARN(const char *message, ...) {
	char output[MAX_LOGGER_MESSAGE_SIZE];
	va_list arg_ptr;
	va_start(arg_ptr, message);
    vsnprintf(output, MAX_LOGGER_MESSAGE_SIZE, message, arg_ptr);
	va_end(arg_ptr);
	printf("\e[1;33m[WARN]:\e[0;33m %s\e[0m\n", output);
}

int main()
{
	void *string_memory = malloc(1024);
	jstring_memory_activate(1024, string_memory);
	jstring_load_logging_function(LOG_WARN);
	jstring_load_logging_function(LOG_WARN);
	return(0);
}
