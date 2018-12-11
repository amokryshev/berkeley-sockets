#include <stdio.h>
#include <sys/socket.h> /* socket(), bind(), connect(), recv(), send() */ 
#include <arpa/inet.h> /* sockaddr_in, inet_ntoa() */
#include <stdlib.h> /* atoi(), exit() */
#include <string.h> /* memset() */
#include <unistd.h> /* close() */
#include <netinet/in.h> 
#define RCVBUFSIZE 4
int client_sock;


static void CrushWithError(char* errorMessage)
{
   printf("The process crush with error: %s", errorMessage);
  
  if(client_sock >= 0) close(client_sock);

  exit(EXIT_FAILURE);
}


int main(int argc, char **argv)
{
	

	char eof[0], echo_string[111], *echo_received, *echo_temp, echo_buffer[RCVBUFSIZE];
	eof[0] = '\0';
	int echo_string_len, recv_msg_size, tail_to_copy;

	struct sockaddr_in serv_address;
	serv_address.sin_family = AF_INET;
	serv_address.sin_port = htons(65535);
	serv_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	
	for(;;)
		{

		printf("Enter the message: ");
		scanf("%s", echo_string);
		if ((client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) CrushWithError("socket() failed");
		if (connect(client_sock, (struct sockaddr*)&serv_address, sizeof(serv_address)) < 0) CrushWithError("connect() failed");
	
		echo_string_len = strlen(echo_string);

		if (send(client_sock, echo_string, echo_string_len, 0) != echo_string_len)
				CrushWithError("send() sent a different number of bytes than expected\n");
		
		
		echo_received = (char*)malloc(0);
		echo_temp = (char*)malloc(0);
		recv_msg_size = 0;
		
		if (send(client_sock, eof, 1, 0) != 1)
				CrushWithError("send() sent a different number of bytes than expected\n");
		
		do { 
			
			if (recv_msg_size != 0) {
				free(echo_temp);
				echo_temp = (char*)calloc(strlen(echo_received), sizeof(char));
				strcpy(echo_temp, echo_received);
			}
			
			if ((recv_msg_size = recv(client_sock, echo_buffer, RCVBUFSIZE, 0)) < 0)
			   CrushWithError("recv() failed");			
			
			if (echo_buffer[0] != eof[0]) {
				
				
				echo_received = (char*)calloc(strlen(echo_temp) + recv_msg_size, sizeof(char));
				tail_to_copy = recv_msg_size;
				
				if(eof[0] == echo_buffer[recv_msg_size-1]) tail_to_copy--;
				if (strlen(echo_temp) > 0) strcpy(echo_received, echo_temp);
				
				free(echo_temp);
				echo_temp = (char*)calloc(tail_to_copy, sizeof(char));
				memcpy(echo_temp, &echo_buffer[0], tail_to_copy);
				strcat(echo_received, echo_temp);
					
			}

		} while (echo_buffer[recv_msg_size - 1] != eof[0]  && recv_msg_size > 0);
		

		printf("The responce is: %s\n", echo_received);
		free(echo_received);
		free(echo_temp);
	}
	
	return 0;
}
