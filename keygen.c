#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc < 2)
    {
        printf("You must provide the size of key\n");
        return EXIT_FAILURE;
    }
    
    int entersize = atoi(argv[1]);
    char canbe_char[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    
    char * redstring = (char*)malloc(entersize * sizeof(char));
    
    for (int i = 0; i < entersize; i++){
        redstring[i] = canbe_char[rand()%27];
    }
    redstring[entersize] = 0;

    printf("%s", redstring);
    free(redstring);
return 0;
}