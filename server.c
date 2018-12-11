#include <stdio.h>
#include <netinet/in.h> 
#define _OE_SOCKETS
#include <sys/types.h>
#include <sys/socket.h> /* socket(), bind(), accept(), connect(), recv(), send() */ 
#include <arpa/inet.h> /* sockaddr_in, inet_ntoa() */
#include <stdlib.h> /* atoi(), exit() */
#include <string.h> /* memset() */
#include <unistd.h> /* close() */
#define MAXPENDING 1
#define RCVBUFSIZE 4

int server_sock, client_sock;

static void CrushWithError(char* errorMessage)
{
  printf("The process crush with error: %s", errorMessage);
  
  if(server_sock >= 0) close(server_sock);
  if(client_sock >= 0) close(client_sock);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{

	char eof, echo_buffer[RCVBUFSIZE];
	eof = '\0';
	unsigned int client_len, recv_msg_size;
	struct sockaddr_in serv_address;
	struct sockaddr_in clnt_address;
	server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	serv_address.sin_family = AF_INET;
	serv_address.sin_port = htons(65535);
	serv_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(bind(server_sock, (struct sockaddr *)&serv_address, sizeof(serv_address))!= -1) {

		if (listen(server_sock, MAXPENDING) < 0) CrushWithError("listen() failed");

		for(;;)
		{
			client_len = sizeof(clnt_address);
			printf("Hold on the accept!\n");
			if ((client_sock = accept(server_sock, (struct sockaddr*)&clnt_address, &client_len))<0)
				CrushWithError("accept() failed");
			printf("Starting recv\n");
			do { 		
				if ((recv_msg_size = recv(client_sock, echo_buffer, RCVBUFSIZE, 0)) < 0)
				   CrushWithError("recv() failed");
				if (send(client_sock, echo_buffer, recv_msg_size, 0) != recv_msg_size)
				   CrushWithError("send() failed");
				printf("Sended msg size: %i\n", recv_msg_size);
			} while (echo_buffer[recv_msg_size - 1] != eof && recv_msg_size > 0);
			
			close(client_sock);
			printf("ECHO HAVE SENDED!\n");
				
		}

	}
	return 0;
}
