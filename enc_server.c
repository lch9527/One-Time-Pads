#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

const char canbe_char[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 

int to_num(char character)
{
	int i;
	for (i = 0; i < 27; i++){
		if (character == canbe_char[i])	{
			return i;
		}
	}

	return -1;
}

char to_char(int num){
    if (num < 0){
        num = 27 + num;
    }
    num = num % 27;
    
	return canbe_char[num];
}




char * encryption(char * text_key){
    char* token = strtok(text_key, "$");
    char *text = (char*)malloc( strlen(token) +1);
    char *ciphertext = (char*)malloc( strlen(token) +1);
    strcpy(text,token);
    token = strtok(NULL, "$");
    char* key = (char*)malloc( strlen(token) +1);
    strcpy(key,token);
	//printf("key : %s\n", key);
    int uc_num, c_num, k_num;
    char c_chat; 
    for (int i = 0; i < strlen(text); i++){
        //printf("text : %c ", text[i]);
        //printf("indx : %d\n", i);
        uc_num = to_num(text[i]);
        k_num = to_num(key[i]);
        c_num = uc_num + k_num;
        c_chat = to_char(c_num);
        ciphertext[i] = c_chat;
    }
    free(text);
    free(key);
    //printf("%s", ciphertext);
    return ciphertext;
}




void conncetion (int connectionSocket){

  char buffer[141000], buffer_2 [141000];
  char s_buffer [1024];
  int charsRead, charsWritten,i = 0;
  memset(buffer, '\0', 141000);
//++++++++++++++++++ recv +++++++++++++++++++
    while (strchr( s_buffer, '@') == NULL){
      memset(s_buffer, '\0', sizeof(s_buffer));
      // Read the client's message from the socket
      charsRead = recv(connectionSocket, s_buffer, sizeof(s_buffer)-1, 0); 
    if (charsRead < 0){
      error("ERROR reading from socket");
      }
      strcat(buffer, s_buffer);
      
      //printf ("buffer : [%s] recv\n", buffer);
    }
//++++++++++++++++++ recv +++++++++++++++++++
    
    strcpy(buffer, encryption(buffer));
    strcat(buffer, "@");
    
//++++++++++++++++++ send +++++++++++++++++++
 memset(s_buffer, '\0', sizeof(s_buffer));
 while (strchr( s_buffer, '@') == NULL){

  memset(s_buffer, '\0', sizeof(s_buffer));
  strncpy(s_buffer, buffer + (256 * i), 256);
  i++;
 //printf("current buffer : [%s]\n", s_buffer);
 charsWritten = send(connectionSocket, s_buffer, strlen(s_buffer), 0); 
 if (charsWritten < 0){
      error("ERROR writing to socket");
    }
  }
//++++++++++++++++++ recv +++++++++++++++++++

    close(connectionSocket);
    // Close the connection socket for this client

  }


// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

void test_connect(int port){
struct sockaddr_in serverAddress, clientAddress;
int connectionSocket;
int charsRead, charsWritten,i = 0;
char buffer[256];
socklen_t sizeOfClientInfo = sizeof(clientAddress);
printf("server test");
int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  setupAddressStruct(&serverAddress, port);

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  listen(listenSocket, 1); 

  connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
      exit(1);
    }
memset(buffer, '\0', 256);
     charsRead = recv(connectionSocket, buffer, sizeof(buffer)-1, 0); 
    if (charsRead < 0){
      error("ERROR reading from socket");
      }

      if (strcmp(buffer, "enc_client") != 0){
      error("Error: could not contact enc_server");
      exit(2);
        }
      
    charsWritten = send(connectionSocket, "enc_server", 11, 0); 
    if (charsWritten < 0){
      error("ERROR writing to socket");
    }
close(listenSocket);
close(connectionSocket);
  
}


int main(int argc, char *argv[]){
  int connectionSocket, charsRead, pid;
  char buffer[141000];
  char buffer_2 [141000];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 

  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  test_connect(atoi(argv[1]));

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 10 connections to queue up
  listen(listenSocket, 10); 
  
  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
      exit(1);
    }

    int  childStatus;
    pid = fork();
		if (pid < 0){
			perror("ERROR fork");
			exit(1);
		}
		if (pid == 0){
			
			conncetion(connectionSocket);
      close(listenSocket);
      
			exit(0);
		}
		else{
      
      pid = waitpid(pid, &childStatus, WNOHANG);
			close(connectionSocket);
		}
  } 
  return 0;
}
