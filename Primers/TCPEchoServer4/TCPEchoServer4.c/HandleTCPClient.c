#include <stdio.h>    /* for fprintf() */
#include <winsock.h>  /* for WSAGetLastError() */
#include <stdlib.h>   /* for exit() */

#define RCVBUFSIZE 32   /* Size of receive buffer */

void DieWithError(char *errorMessage)  /* Error handling function */
{
	fprintf(stderr, "%s: %d\n", errorMessage, WSAGetLastError());
	exit(1);
}

void HandleTCPClient(int clntSocket)
{
	char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
	int recvMsgSize;                    /* Size of received message */

	memset(&echoBuffer, 0, sizeof(echoBuffer));
	/* Receive message from client */
	if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
		DieWithError("recv() failed");

	printf("Received: %d Bytes.\nReceived: ", strlen(echoBuffer));
	puts(echoBuffer);

	/* Send received string and receive again until end of transmission */
	while (recvMsgSize > 0)      /* zero indicates end of transmission */
	{
		/* Echo message back to client */
		if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
			DieWithError("send() failed");

		/* See if there is more data to receive */
		if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
			DieWithError("recv() failed");
	}

	closesocket(clntSocket);    /* Close client socket */
}