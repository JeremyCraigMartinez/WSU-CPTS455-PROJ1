/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

char* StringNewlineToNullTerminator(char* string)
{
	char* targetChar = strchr(string, '\n');
	if (targetChar == 0)
		return 0; // Return nullptr, this is bad.
	else
	{
		*targetChar = 0; // Null terminate the buffer to a C-string.
		return string; //Return the first address of the string
	}
}

char* StringNullToNewlineTerminator(char* string)
{
	char* targetChar = strchr(string, 0);
	if (targetChar == 0)
		return 0; // Return nullptr, this is bad.
	else
	{
		*targetChar = '\n'; // Newline terminate the buffer to a TCP string.
		return string; //Return the first address of the string
	}
}

int check(char *string, char* creds) {
    int second_newline_char = 0;
    while (second_newline_char < 2) {
        if (!(*string == *creds))
	    return 0;
        if (*string == '\n') second_newline_char++;
        string++;
        creds++;
    }
    return 1;
}

char* credentials = "12345678\nhauser\n";
char* pass = "Password1\n";

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    while(1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        char welcome[256] = "welcome";
        StringNullToNewlineTerminator(welcome);

        if (newsockfd > 0) {
            n = write(newsockfd,welcome,8);
        }
        if (newsockfd < 0) 
            error("ERROR on accept");

        //username and ID
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");
        printf("Here is the message: %s\n",buffer);

        printf("strcmp: %s==%s:%d\n",buffer,credentials,check(buffer,credentials));
	int fail;
        if (check(buffer,credentials)==1) {
        char success[256] = "SUCCESS";
        StringNullToNewlineTerminator(success);
            n = write(newsockfd,success,8);
	    fail = 0;
        }
        else {
	    fail = 1;
	    char failure[256] = "FAILURE";
	    StringNullToNewlineTerminator(failure);
            n = write(newsockfd,failure,8);
        }

        if (!(fail)) {

        //now the password length
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");
        printf("Here is the message: %s\n",buffer);

        short length = atoi(buffer);
	length = ntohs(length);

        //now the password
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");
        printf("Here is the message: %s\n",buffer);

        printf("strcmp: %s==%s:%d\n",buffer,pass,strncmp(buffer,pass,length));
        if (strncmp(buffer,pass,length)==0) {
            char success[256];
	    bzero(success,256);
	    sprintf(success,"congratulations hauser, you just revealed your password to the world");
	    printf("%s",success);
            StringNullToNewlineTerminator(success);
	    char success_len[5];
	    bzero(success_len,5);
	    short str_len = strlen(success);
	    str_len = htons(str_len);
	    sprintf(success_len,"%d",str_len);
	    printf("%s",success_len);
	    n = write(newsockfd,success_len,5);
            n = write(newsockfd,success,str_len);
        }
        else {
            close(newsockfd);
        }
        }
        if (n < 0) error("ERROR writing to socket");
    }
    return 0; 
}
