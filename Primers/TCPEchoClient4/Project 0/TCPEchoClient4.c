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

int main(int argc, char * argv[])
{
#pragma region argument handling
	if (argc < 3 || argc > 4)
		DieWithError("Parameter(s)", "<Server Adress> <Echo Word> [<Server Port>]");

	char *servIP = argv[1]; //First arguement: server IP address in dotted quad format
	char *echoString = argv[2]; // Second argument: string to echo
	in_port_t servPort = (argc == 4) ? atoi(argv[3]) : 7; // Third argument (optional): 7, an echo port, otherwise its what the user input
#pragma endregion

#pragma region TCP socket creation
	WORD wVersionRequested = MAKEWORD(2, 2); // Windows garbage, this is to tell winsock.dll to be version 2.2
	WSADATA wsaData; // Place holder address for winsock.dll data.
	
	int winsock = WSAStartup(wVersionRequested, &wsaData); // Initiates winsock.dll as another process
	if (winsock != 0)
		DieWithError("WSAStartup failed with error: %d");
	else
		printf("WSAStartup() succeeded. winsock = %d\n", winsock);

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Creates a reliable stream socket using TCP
	if (sock < 0) // socket() failed
		DieWithError("socket() failed");
	else
		printf("socket() succeeded. sock = %d\n", sock);
#pragma endregion

#pragma region Server address construction/conversion
	struct sockaddr_in servAddr; // Server address
	memset(&servAddr, 0, sizeof(servAddr)); //Zero out servAddr
	servAddr.sin_family = AF_INET; // IPv4 address family
	int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr); // converts servIP to an IPv4 (AF_INET) formatted binary number
	if (rtnVal == 0) // rtnVal == 1 on succesful conversion
		DieWithError("inet_pton() failed", "invalid address string");
	else if (rtnVal < 0)
		DieWithError("inet_pton() failed");

	printf("Server port: %d\n", servPort);
	servAddr.sin_port = htons(servPort); // Server port is converted from little endian (host order) to big endian format in memory. (htons = Hex to Network Structure)
#pragma endregion

#pragma region Send the input string to the server
	int connectErr = connect(sock, (struct sockaddr*) &servAddr, sizeof(servAddr));
	printf("conectErr: %d\n", WSAGetLastError());
	if (connectErr < 0) // Establish connection to echo server
		DieWithError("connect() failed");
	printf("connect() successful\n");

	SIZE_T  echoStringLen = strlen(echoString); // Determine input string length
	SSIZE_T numBytes = send(sock, echoString, echoStringLen, 0); // numBytes is the number of bytes sent.

	if (numBytes < 0) // Verify all of string sent.
		DieWithError("send() failed");
	else if (numBytes != echoStringLen)
		DieWithError("send()", "sent unexpected number of bytes");
#pragma endregion

#pragma region Receive string from the server
	u_int totalBytesReceived = 0;
	fputs("Received: ", stdout);
	while (totalBytesReceived < echoStringLen) // Continuosly pull bytes of sent string until received bytes >= string length.
	{
		char buffer[BUFSIZE]; // I/O buffer
		numBytes = recv(sock, buffer, BUFSIZE - 1, 0); // Receives up to bufsize - 1 to terminate string.
		printf("%d Bytes\n", numBytes);
		if (numBytes < 0)
			DieWithError("recv() failed");
		else if (numBytes == 0)
			DieWithError("recv()", "connection closed prematurely");
		totalBytesReceived += numBytes; // Keep tally of total bytes
		buffer[numBytes] = '\0'; // Terminate the string
		fputs("Received: \"", stdout); // Print the buffer
		fputs(buffer, stdout); // Print the buffer
	}
	fputs("\"\n", stdout); // Print final line feed
#pragma endregion

#pragma region Housekeeping
	closesocket(sock); // Free up socket
	WSACleanup(); // Kill winsock.dll initiation
	return 0;
#pragma endregion

}
