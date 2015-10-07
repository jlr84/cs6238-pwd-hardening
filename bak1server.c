/* server.c
   Author: James Roberts
   CS6238 Project 1: Authenticated Encryption
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "crypt.c" //To test this file "standalone" use 'crypt_proof.c'
#include "tokenize.c" //To test this file "standalone" use 'tokenize_proof.c'

/* ERROR Codes, as structured:
111 = invalid input file; history file not found or not opened successfully;
222 = error during history file validation
*/

//Program Constants:
char const historyFolderLocation[] = "./history"; // ensure path here is also on below line
char const historyFilePath[] = "./history/historyfile"; // ensure path matches above line
char const validationString[] = "VALID\n";
char const endFileString[] = "END_OF_FILE\n";
int const historyFileSize = 1200;

//Type Definitions:
typedef struct{
    char *validation;
    char *size;
    char *line1;
    char *line2;
    char *line3;
    char *line4;
    char *line5;
    char *end;
}history;

//Function Declarations:
int process_history(history currentHistory);
int createFreshHistory ();

//MAIN-----------------------------------------------
int main(int argc, char* argv[])
{
    //Initialize Program Status
    int status = 0; 
    //Declare current history struct
    history currentHistory; 

    //Create Fresh history directly -- for testing
//    createFreshHistory();

    //Process history file
    status = process_history(currentHistory);
    
    return status;

}

// Function processing history file stored in fixed location 
int process_history(history currentHistory) {
    int history_status = 0;
    FILE* inputfile = fopen(historyFilePath, "r");
    unsigned char decryptedtextbuffer[5000];
    unsigned char *decryptedtextptr = (unsigned char *)&decryptedtextbuffer;

    if ( inputfile == 0) {
        history_status = 111;
        //fopen returns 0, the NULL pointer, on failure
        perror("History File Missing");
        
        //Create new history file
        createFreshHistory();
        return history_status;
    }
    else {
	decrypt_file_to_buf(inputfile, decryptedtextptr);
    	char mybufin[5000];
        
        //Copy decryptedbuffer to mybufin[]
	strcpy(mybufin, decryptedtextptr);

        printf("Starting Buffer:\n%s\n\n", mybufin);

        //Tokenize our input buffer
        char **tokens;
        tokens = str_split(mybufin, '\n');
        if (tokens) {
            currentHistory.validation = *(tokens + 0);
            currentHistory.size = *(tokens + 1);
            currentHistory.line1 = *(tokens + 2);
            currentHistory.line2 = *(tokens + 3);
            currentHistory.line3 = *(tokens + 4);
            currentHistory.line4 = *(tokens + 5);
            currentHistory.line5 = *(tokens + 6);
            currentHistory.end = *(tokens + 7);
        }
 
        //Print to screen for validation/testing
        printf("CurrentHistory Stored Successfully: \nVAL:%s\nSIZE:%s\nLN1:%s\nLN2:%s\nLN3:%s\nLN4:%s\nLN5:%s\nEND:%s\n",currentHistory.validation,currentHistory.size,currentHistory.line1,currentHistory.line2,currentHistory.line3,currentHistory.line4,currentHistory.line5,currentHistory.end);

        //Since the file opened successfully, close it here
        fclose(inputfile);
    }
    
    /* Compare Results to validate proper formatting; if invalid formatting, 
       assume corrupted file or incorrect decrypting */
    if ( strcmp(currentHistory.validation,"VALID") == 0 && strcmp(currentHistory.end,"END_OF_FILE") == 0 ) {
        printf("\nVALIDATED SUCCESSFULLY\n");
    } else {
        printf("\nHistory File Error, or Invalid Decryption\n");
        history_status = 222;
    }
    return history_status;
}

//Function for creating a new blank history file when was is not already present
int createFreshHistory () {
    FILE *historyFile; 
    int returncode = 0;

    //Make directory if not already present on system
    struct stat st ={0};
    if (stat(historyFolderLocation, &st) == -1) {
        mkdir(historyFolderLocation, 0700);
    }
    
    //Open file
    historyFile = fopen(historyFilePath, "w+");
    
    //Write Base structure
    unsigned char mybufin[1000] = "VALID\n0\nNA\nNA\nNA\nNA\nNA\nEND_OF_FILE\n";
    unsigned char filler[] ="aaaa";

    //Pad to length=1000
    int i;
    int length = strlen( mybufin );
    for ( i = length; i < 1000; i = i + 1) {
        mybufin[i] = filler[1];
    }
    
    //Create pointer to input buffer just created
    unsigned char *mybufinptr = (unsigned char *)&mybufin;

    //Encrypt buffer and write to file
    printf("\nBEGINNING ENCRYPTION; Starting with...\nInput Buffer: %s\n",mybufinptr);
    printf("ENCRYPTING BUFFER TO FILE %s\n", historyFilePath);
    returncode = encrypt_buf_to_file(mybufinptr, historyFile);
    fclose(historyFile);
    printf("ENCRYPTION COMPLETED\n");
    printf("\nFresh history file created.\n");
}
