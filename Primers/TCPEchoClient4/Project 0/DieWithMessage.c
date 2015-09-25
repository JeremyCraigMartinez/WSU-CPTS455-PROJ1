#include <stdio.h>    /* for fprintf() */
#include <winsock.h>  /* for WSAGetLastError() */
#include <stdlib.h>   /* for exit() */

void DieWithError(char *errorMessage)
{
	fprintf(stderr, "%s: %d\n", errorMessage, WSAGetLastError());
	exit(1);
}