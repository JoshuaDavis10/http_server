#include "string_library/jstring.h"
#include "util.c"
#include "socket.c"

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

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

jstring create_http_response_html(const char *response, u64 size)
{
	jstring http_response = jstring_create_temporary(
			response, size);
	/* TODO: string length function instead of hardcoding string size */
	jstring http_header = jstring_create_temporary(
			"HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ",
			56);
	jstring http_content_length_string = 
		jstring_create_integer(size);

	jstring_concatenate_jstring(&http_header, http_content_length_string);
	jstring_concatenate_raw(&http_header, "\n\n");
	jstring_concatenate_jstring(&http_header, http_response);
	return http_header;
}

int main() {

	/* TODO: create a function to read an entire file into a 
	 * string 
	 */
	/* read form.html from file */
	struct stat st;
	i64 form_html_size;
	if(stat("form.html", &st) == -1)
	{
		LOG_ERROR("stat returned -1 for 'form.html'");
		return -1;
	}
	form_html_size = st.st_size;

	LOG_DEBUG("form.html size: %d", form_html_size);

	char *form_html_string = 
		malloc(form_html_size+1);

	i32 form_html_fd = open("form.html", O_RDONLY, "rb");
	read(form_html_fd, form_html_string, form_html_size);
	form_html_string[form_html_size] = '\0';

	/* read submitted.html from file */
	i64 submitted_html_size;
	if(stat("submitted.html", &st) == -1)
	{
		LOG_ERROR("stat returned -1 for 'submitted.html'");
		return -1;
	}
	submitted_html_size = st.st_size;

	LOG_DEBUG("submitted.html size: %d", submitted_html_size);

	char *submitted_html_string = 
		malloc(submitted_html_size+1);

	i32 submitted_html_fd = open("submitted.html", O_RDONLY, "rb");
	read(submitted_html_fd, submitted_html_string, submitted_html_size);
	submitted_html_string[submitted_html_size] = '\0';

	/* set up jstring stuff */
	void *string_memory = 
		malloc(form_html_size * 10 + submitted_html_size * 10);
	memset(string_memory, '-', 
			form_html_size * 10 + submitted_html_size * 10);
	if(!jstring_memory_activate(
				form_html_size * 10 + submitted_html_size, 
				string_memory))
	{
		LOG_ERROR("failed to setup jstring library's memory");
		return -1;
	}
	jstring_load_logging_function(LOG_DEBUG);

	/* create http_response jstrings */
	jstring http_response_form = 
		create_http_response_html(form_html_string, form_html_size);
	free(form_html_string);
	jstring http_response_submitted = 
		create_http_response_html(submitted_html_string, 
				submitted_html_size);
	free(submitted_html_string);

	i32 sock = socket_create();
	i32 client = socket_accept(sock);

	/* TODO: temp, need to be able to parse client responses to know
	 * which messages to send
	 */
	b8 form_sent = false;
	char message[2048];
	while(true) 
	{
		i8 result = socket_poll(client);
		switch(result)
		{
			case -1:
			{
				_assert(0);
				/* TODO: disconnect instead of assert */
			} break;
			case 2:
			{
				_assert(0);
				/* TODO: disconnect instead of assert */
			} break;
			case 1:
			{
				socket_recv(message, 2048, client);
				LOG_DEBUG("%s", message);
				/* TODO: parse message and respond accordingly */
				if(!form_sent)
				{
					socket_send(http_response_form.data, client);
					form_sent = true;
				}
				else
				{
					socket_send(http_response_submitted.data, client);
					form_sent = false;
				}
			} break;
			case 0:
			{
				/* nuthin' */
			} break;
			default:
			{
				/* NOTE: this should never happen */
				_assert(0);
			} break;
		}
	}
	return 0;
}
