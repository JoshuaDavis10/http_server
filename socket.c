#include "socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>

#include <string.h> /* TODO: replace with string library function 
					 * (strlen, strcpy)
					 */

#define MAX_BACKLOG 5 
#define PORT "8080"

void* get_in_addr(struct sockaddr *sa) {
    if(sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

b8 socket_send(const char* in_message, i32 fd)
{
	i32 result = send(fd, in_message, strlen(in_message), 0);
	if(result == -1) {
		LOG_ERROR("%s: send returned -1", __FUNCTION__);
		return false;
	}
	LOG_TRACE("%s: sent message:\n\n%s", __FUNCTION__, in_message);
	return true;
}

b8 socket_recv(char* out_message, i32 size, i32 fd) 
{
	i32 bytes = recv(fd, out_message, size, 0);
	if(bytes == -1 || bytes >= size) {
		LOG_ERROR("%s: recv returned -1", __FUNCTION__);
		return false;
	}
	out_message[bytes] = '\0';
	LOG_TRACE("%s: got message:\n\n%s", __FUNCTION__, out_message);
	return true;
}

i8 socket_poll(i32 fd) 
{
	struct pollfd tmp;
	tmp.fd = fd;
	tmp.events = POLLIN;
	i32 result = poll(&tmp, 1, 0);
	if(result > 0) {
		if(tmp.revents & POLLHUP) {
			return 2;
		}
		if(tmp.revents & POLLIN) { 
			return true;
		}
	}
	if(result < 0) { /* error */
		return -1;
	}
	return false;
}

i32 socket_accept(i32 fd) {
	return accept(fd, NULL, NULL);
}

i32 socket_create() {

    /* declarations for socket */
    struct addrinfo hints;      /* will be used as 3rd param in 
								   getaddrinfo() (give getaddrinfo 
								   'hints' about how u want to use it?) 
								 */
    struct addrinfo* servinfo;  /* pass as 4th param of getaddrinfo 
								   to get the linked list of addrinfo 
								   structs
								 */

    memset(&hints, 0, sizeof(struct addrinfo)); /* zero out memory */
    hints.ai_family = AF_UNSPEC;        /* don't care IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;    /* TCP stream */
    hints.ai_flags = AI_PASSIVE;        /* fill in IP for me 
										   (don't have to hardcode it)
										 */

    /* getaddrinfo() */
    /* ig getaddrinfo should return 0 if its successful */
    if((getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        printf("\e[1;31mERROR: getaddrinfo() did not return 0 as "
				"expected\e[0;37m\n");
        return -1;
    }

    /* loop through addrinfo linked list until we can create and 
	 * bind a socket with it */
    struct addrinfo* ai_ptr;
    i32 sockfd; /* socket ID? */
    i32 yes = 1; /* for setsockopt()... 4th param */
    for(ai_ptr = servinfo; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next) {
        /* socket() */
        if((sockfd = socket(ai_ptr->ai_family, ai_ptr->ai_socktype, 
						ai_ptr->ai_protocol)) == -1) {
            printf("\e[1;33mWARNING: socket() returned -1\e[0;37m\n");
            continue; /* go to next addrinfo struct in linked list */
        }
        /* setsockopt() <- allows to rerun server without having to 
		 * wait a minute for the socket to clear after last run
		 */
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
					&yes, sizeof(i32)) == -1) {
            printf("\e[1;31mERROR: setsockopt() returned -1\e[0;37m\n");
            return -1;
        }
        /* bind() <- associate socket with an actual port */
        if(bind(sockfd, ai_ptr->ai_addr, ai_ptr->ai_addrlen) == -1) {
            close(sockfd); /* close the socket we created since 
							  we can't bind it 
							*/
            printf("\e[1;33mWARNING: bind() returned -1\e[0;37m\n");
            continue; /* go to next addrinfo struct in linked list */
        }
        
        break; /* end loop once we've successfully created/bound 
				  a socket 
				*/
    }

    freeaddrinfo(servinfo); /* done with addrinfo struct */

    /* check that socket was actually bound */
    if(ai_ptr == NULL) {
        printf("\e[1;31mERROR: failed to bind\e[0;37m\n");
        return -1;
    }

    /* listen() <- wait for incoming connections with */
    if(listen(sockfd, MAX_BACKLOG) == -1) {
        printf("\e[1;31mERROR: listen() returned -1\e[0;37m\n");
        return -1;
    }

    return sockfd;

}
