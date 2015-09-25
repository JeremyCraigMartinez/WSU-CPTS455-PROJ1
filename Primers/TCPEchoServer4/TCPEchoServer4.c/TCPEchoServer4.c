#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
//#include <sys\types.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <basetsd.h>
#include "Practical.h"
#pragma comment(lib, "Ws2_32.lib")

static const int MAXPENDING = 5; // Maximum outstanding connection requests

int main(int argc, char * argv[])
{
#pragma region argument handling
	if (argc != 2)
		DieWithError("Parameter(s) <Server Port>");

		in_port_t servPort = atoi(argv[1]); // First argument: local port
#pragma endregion

#pragma region TCP socket creation for incoming connection
	WSADATA wsaData; // Place holder address for winsock.dll data.

	int winsock = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initiates winsock.dll as another process
	if (winsock != 0)
		DieWithError("WSAStartup failed with error: %d");
	else
		printf("WSAStartup() succeeded. winsock = %d\n", winsock);

	SOCKET servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Creates a reliable stream socket using TCP
	if (servSock < 0) // socket() failed
		DieWithError("socket() failed");
	else
		printf("socket() succeeded. sock = %d\n", servSock);
#pragma endregion

#pragma region Construct local address structure
	struct sockaddr_in servAddr; // Local address
	memset(&servAddr, 0, sizeof(servAddr)); // Zero out servAddr structure
	servAddr.sin_family = AF_INET; // IPv4 address family.
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY: allows program to work without knowing its own IP address
	//htonl() converts INADDR_ANY from host byte order (little endian) to network byte order (big endian). (htonl = Host to Network Long)
	servAddr.sin_port = htons(servPort); // Server port is converted from little endian (host order) to big endian format in memory. (htons = Host to Network Short)
#pragma endregion

#pragma region Local server address and socket handling
	if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) // Assigns the server's address and name to the server socket
		DieWithError("bind() failed");
	if (listen(servSock, MAXPENDING) < 0) // Mark the server's socket so it will listen for incoming connections
		DieWithError("listen() failed");
#pragma endregion

#pragma region Connection handling
	for (;;)
	{
		struct sockaddr_in clientAddr; // Address of incoming client
		socklen_t clientAddrLen = sizeof(clientAddr);

		int clientSock = accept(servSock, (struct sockaddr *) &clientAddr, &clientAddrLen); // The server waits until a client attempts to connect
		if (clientSock < 0) // Server/client connection failed
			DieWithError("accept() failed");
		//else - Server/client connected!
		char clientName[INET_ADDRSTRLEN]; // String that contains the client address
		if (inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientName, sizeof(clientName)) != NULL)
			printf("Handling client %s/%d\n", clientName, ntohs(clientAddr.sin_port));
		else
			puts("Unable to get client address");

		HandleTCPClient(clientSock);
	}
#pragma endregion

#pragma region Housekeeping (never really get here)
	closesocket(servSock); // Free up socket
	WSACleanup(); // Kill winsock.dll initiation
	return 0;
#pragma endregion

}
