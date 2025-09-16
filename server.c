#include "util.c"
#include "socket.c"

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main() {

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
	LOG_DEBUG("form_html_string: %s", form_html_string);
	/* TODO: append HTTP header and store that in a buffer */

	/*
	 * NOTE: http header stuff
	 * HTTP/1.1 200 OK
	 * Content-Type: text/html
	 * Content-Length: <length>
	 */

	/* TODO: one day I will write a string library that doesn't use
	 * c standard library somehow
	 */
	char http_header[2048];
	snprintf(http_header, 2048, 
			"HTTP/1.1 200 OK\n"
			"Content-Type: text/html\n"
			"Content-Length: %lld\n\n",
			form_html_size);
	i32 http_header_size = strlen(http_header);	
	char *http_message_string =
		malloc(form_html_size + http_header_size + 1);
	strcpy(http_message_string, http_header);
	strcat(http_message_string, form_html_string);

	LOG_DEBUG("http_response:\n%s", http_message_string);

	i32 sock = socket_create();
	i32 client = socket_accept(sock);

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
				/* TODO: parse message and respond accordingly */
				socket_send(http_message_string, client);
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
