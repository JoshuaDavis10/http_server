#pragma once

#define MAX_MESSAGE_SIZE 4096

#include "defines.h"

/*
 * creates a socket, binds it and listens on it
 * (i.e. does all the socket setup code)
 *
 * @return: returns the fd of the socket
 */
i32 socket_create();

/*
 * wrapper for accept().
 * performs a blocking operation while waiting 
 * for a connection on socket 'fd'
 *
 * @return: returns fd of connection
 */
i32 socket_accept(i32 fd); 

/*
 * @param: 'in_message': the message to be sent 
 * @param: 'fd': the file descriptor to send the message to
 *		 (i.e. client to send it to)
 * @return: return true on success, false on failure
 */
b8 socket_send(const char* in_message, i32 fd);

/*
 * @param: 'out_message': pointer to a buffer to copy the received message into
 * @param: 'size': size of the buffer pointed to by out_message
 * @param: 'fd': file descriptor to receive the message from
 *
 * @return: returns true on success, false on failure
 */
b8 socket_recv(char* out_message, i32 size, i32 fd);

/*
 * checks socket (non-blocking) for message
 *
 * @param: 'fd': file descriptor to poll
 *
 * @return: returns 1 (true) if there is a message to receive
 *	    returns 0 (false) if there is no message to receive
 *	    returns 2 if the socket has been closed (disconnection)
 *	    returns -1 on error
 */
i8 socket_poll(i32 fd);
