#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
const char canbe_char[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
 
 char* hostname = "localhost";
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  printf("name : [%s]\n",hostInfo->h_name);
  fflush(stdout);
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

char *get_file_string(char *filePath){
  FILE *fp = fopen(filePath, "rb");
  if (!fp){
		printf("Can't open file %s\n", filePath);
		exit(1);
	}

  fseek(fp, 0L, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  char *str = (char*)malloc(size +1);
  fread(str, size, 1, fp);
  fclose(fp);
  return str;
}

void check_input(char *txt, char *key){
  if (strlen(txt) > strlen(key)){
    error("too-short key");
    exit(1);
  }
  for (int i = 0; txt[i]; i++){
    if (strchr(canbe_char, txt[i]) == NULL){
      
      printf(" at : [%c]\n",txt[i]);
      error("invalid char");
       exit(1);
    }
  }
}

void test_connect(int port){
int FD;
int charsWritten, charsRead;
char buffer[256];
struct sockaddr_in serverAddress;
 FD = socket(AF_INET, SOCK_STREAM, 0); 
  if (FD < 0){
    error("CLIENT: ERROR opening socket");
  }
   // Set up the server address struct
  setupAddressStruct(&serverAddress, port);
  // Connect to server
  if (connect(FD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }

printf("client test\n");
  charsWritten = send(FD,  "dec_client", 11, 0);
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
    }
  charsRead = recv(FD, buffer, sizeof(buffer) - 1, 0); 
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
  if (strcmp(buffer, "dec_server") != 0){
    error("Error: could not contact enc_server");
    exit(2);
  }
close(FD);
}


int main(int argc, char *argv[]) {
  int socketFD, portNumber, charsWritten, charsRead, i = 0;
  char key_2 [141000];
  struct sockaddr_in serverAddress;
  char buffer [141000];
  char s_buffer [1024];
  char out_buffer[141000];
  // Check usage & args
  if (argc != 4) { 
    fprintf(stderr,"USAGE: enc_client myplaintext key port"); 
    exit(0); 
  } 
// text key port
// get text
  char *plaintext = get_file_string(argv[1]);
// get key
  char *key = get_file_string(argv[2]);
// get port
  int port = atoi(argv[3]);

  long int sent_chars = 0;

  
  // Remove the trailing \n that fgets adds
  plaintext[strcspn(plaintext, "\n")] = '\0';
  check_input(plaintext,key);
  plaintext = strcat(plaintext,"$");
  strcpy(buffer, plaintext);

 
  strncpy(key_2, key, 1024);
  memcpy(key_2, key ,strlen(buffer)+1);
  strcat(key_2,"@");
  strcat(buffer, key_2);

  //strncpy(s_buffer, buffer + 1, 256);
  //printf("[%s] \n", s_buffer);

  //printf ("text : %s, key : %s , port : %d\n", plaintext, key_2, port);
  test_connect(port);
// Create a socket

 socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
  setupAddressStruct(&serverAddress, port);

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }

  
  // Send message to server
  // Write to the server
  
 
//printf("current buffer : [%s]\n", buffer);
/*
while (strchr( s_buffer, '@') == NULL){
 memset(s_buffer, '\0', sizeof(s_buffer));
  strncpy(s_buffer, buffer + (256 * i), 256);
  i++;

  printf("%s", s_buffer);
  
  }
*/

  while (strchr( s_buffer, '@') == NULL){
  memset(s_buffer, '\0', sizeof(s_buffer));
  strncpy(s_buffer, buffer + (256 * i), 256);
  i++;
  charsWritten = send(socketFD, s_buffer, strlen(s_buffer), 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
    }
  }



  // Get return message from server
  // Clear out the buffer again for reuse
  memset(buffer, '\0', sizeof(buffer));

  // Read data from the socket, leaving \0 at end
  while (strchr( buffer, '@') == NULL){

  memset(s_buffer, '\0', sizeof(s_buffer));
  charsRead = recv(socketFD, s_buffer, sizeof(s_buffer) - 1, 0); 
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
    strcat(buffer, s_buffer);
  }
  buffer[strlen(buffer)-1] = '\0';
  printf("%s", buffer);


/*
//send loop
while(sent_chars < strlen(buffer)){
  int sent = send(socketFD, buffer + sent_chars, strlen(buffer) - sent_chars, 0);
  if (sent< 0){
    //error
    break;
  }
  sent_chars += sent;
}

//receive loop
while(strstr(out_buffer, "@") == NULL){
  int read = 0;
  //empty buffer at every loop
  char t_buffer[142000];
  memset(t_buffer, 0, sizeof(buffer));
  read = recv(socketFD, t_buffer, sizeof(t_buffer), 0);
  if (read < 0){
    //error
    break;
  }
 strcat(out_buffer, t_buffer); 
}
*/


  // Close the socket
  close(socketFD); 

  return 0;
}