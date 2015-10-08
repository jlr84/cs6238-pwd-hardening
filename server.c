/* server.c
   Author: James Roberts
   CS6238 Project 1: Authenticated Encryption
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "crypt.c" //crypt_standalone.c is the same with 'main' added for testing
#include "tokenize.c" //tokenize_standalone.c is the same with 'main' added for testing
#include "utilities.h"
#include "server.h"

/* ERROR Codes, as structured:
111 = history file not found or not opened successfully;
222 = error during history file validation
333 = input file not found
*/


//Program Constants:
char const historyFolderLocation[] = "./history"; // ensure path here is also on below line
char const historyFilePath[] = "./history/historyfile"; // ensure path matches above line
char const validationString[] = "VALID\n";
char const endFileString[] = "END_OF_FILE\n";
int const historyFileSize = 1200;


//Function for creating a new blank history file when it was not already present
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
    //printf("\nBEGINNING ENCRYPTION; Starting with...\nInput Buffer: %s\n",mybufinptr);
    printf("Encrypting 'empty' history to file '%s'\n", historyFilePath);
    returncode = encrypt_buf_to_file(mybufinptr, historyFile);
    fclose(historyFile);
    printf("Encryption Completed\n");
    printf("Fresh history file created.\n");
}


// Function processing history file stored in fixed location 
int process_history(history currentHistory) {
    int history_status = 0;
    FILE* inputfile = fopen(historyFilePath, "r");
    unsigned char decryptedtextbuffer[5000];
    unsigned char *decryptedtextptr = (unsigned char *)&decryptedtextbuffer;

    printf("\nProcessing History File:\n");

    if ( inputfile == 0) {
        history_status = 111;
        //fopen returns 0, the NULL pointer, on failure
        perror("\nHISTORY FILE MISSING");
        
        //Create new history file
        createFreshHistory();
        inputfile = fopen(historyFilePath, "r");
    }
    
    decrypt_file_to_buf(inputfile, decryptedtextptr);
    char mybufin[5000];
        
    //Copy decryptedbuffer to mybufin[]
    strcpy(mybufin, decryptedtextptr);

    printf("\nHistory File Decrypted; starting Buffer:\n%s\n\n", mybufin);

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
        
    /* Compare Results to validate proper formatting; if invalid formatting, 
       assume corrupted file or incorrect decrypting */
    if ( strcmp(currentHistory.validation,"VALID") == 0 && strcmp(currentHistory.end,"END_OF_FILE") == 0 ) {
        printf("\nHistory File Validated Successfullly\n");
    } else {
        printf("\nHistory File Error, or Invalid Decryption\n");
        history_status = 222;
    }
    return history_status;
}

//PLACEHOLDER FOR Pr Function
int PR(int r, int x) {
    int y = x*r;
    return y;
}

//PLACEHOLDER FOR Gr Function
int GR(int r, int x, char* pswd) {
    int y = x*r+strlen(pswd);
    return y;
}

//Function for calculating all alpha column values as valid
void computeAlpha(long* alpha, int numfeatures, char* password, int q, int r) {

    int i;
    for (i = 1; i <= numfeatures; i++) {
        alpha[i-1] = PR(r, i*2) + ( GR(r, i*2, password) % q );
    }
    printf("Alpha Column initialized.\n");
};

//Function for calculating all alpha column values as valid
void computeBravo(long* bravo, int numfeatures, char* password, int q, int r) {

    int i;
    for (i = 1; i <= numfeatures; i++) {
        bravo[i-1] = PR(r, i*2+1) + ( GR(r, i*2+1, password) % q );
    }
    printf("Bravo Column initialized.\n");
};

//Verifies each password/feature pair; returns 1 if good password; returns 0 if bad password
int verifyPassword(char* pwd, char* feats) {
    
    int status = 0;

    history currentHistory; 			//Declare current history struct
    status = process_history(currentHistory);	//process history file

    //ADD LOGIC HERE

    return 1;

}


//Processes input file from beginning to end; calls verifyPassword() for each password/feature pair
int processInput(char* argv[], char* pwd, char* feats) {

    //Open Input File
    FILE *f = fopen(argv[1], "r");
    //Open Output File
    FILE *output = fopen(argv[2], "w+");

    char buff[200]; //Extra/temporary buffer used during transfer
    int status = 0;
    int verified;

    if (!f) {
        printf("INPUT FILE NOT FOUND\n");
	status = 333;
    } else {
        int i = 1;
        while (fgets(buff, 200, f) != NULL) {
            if (i % 2) {
                //If i is odd do this:
                strcpy(pwd, buff);
                printf("ODD: %s\n",buff);
                //Password i has now been copied out.

            } else {
                //Else, i.e., if i is even do this:
                strcpy(feats, buff);
                printf("EVEN: %s\n", buff);
                //Features i have now been copied out; time to verify
                verified = verifyPassword(pwd, feats);
                printf("STATUS: %d\n",status);
                if (verified == 1) {
                    fputs("1\n", output);
                } else {
                    fputs("0\n", output);
                }
                printf("Password/Feature #%d verified\n", i/2);
            }
            i = i + 1;
        }
        printf("End of input file.\n");
    }
    fclose(f);
    fclose(output);
    printf("Function readInput() complete.\n");
    return status;
}


//Runs program initialization sequence
int initProgram(char* argv[]) {
    int status = 0;
    /////////////////////////////
    //INITIALIZATION
    /////////////////////////////
    printf("Beginning Initialization.\n");

    /* FIRST, get initial password and features from input file */
    //Open Input File, then get first line (password)
    FILE *f = fopen(argv[1], "r");
    char buff[200]; //Extra/temporary buffer used during transfer
    if (!f) {
        printf("INPUT FILE NOT FOUND\n");
        status = 333;
    } else {
        fgets(buff, 200, f);
    }
    int pwdlen = strlen(buff);
    char password[pwdlen];
    pwdlen = copy_pwd(password, buff);
    printf("Password:>>>%s<<<\nLength: %d\n", password, pwdlen);
    //Get second line (features)
    fgets(buff, 200, f);
    int fealen = strlen(buff);
    char featurestr[fealen];
    char *featureString = featurestr;
    fealen = copy_pwd(featureString, buff);
    printf("Features: %s\n", featureString);
    int features[127];
    int numfeatures = str_to_ints(featureString, features);
    printf("Number of Features: %d\n",numfeatures);
    int i;
    printf("Features Stored as Integers:\n|");
    for ( i = 0; i < numfeatures; i++) {
        printf(" %d |",features[i]);
    }


    /* SECOND, select 160-bit prime value q; also select random Hpwd where
       Hpwd < q; note, h is fixed at 5; also select 160-bit value r that 
       will be stored for future use after initialization */
    //INSERT FUNCTION HERE
    int q = 33;  // temporary
    printf("\nq: %d\n",q);
    int r = 42;  // temporary
    printf("r: %d\n",r);


    /* THIRD, determine Maximum number of distinguising features, m,
       then select polynomial f of degree m-1 */
    printf("Distinguishing Features: %d\n", numfeatures);
    printf("Selecting Polynomial of degree: %d\n", numfeatures-1);
    //INSERT FUNCTION HERE
    //Select polynomial f of degree m-1
    //f(0) will give us our initial hpwd
    long hpwd = 1234567890; //temporary; use f polynomial
    printf("Random hpwd: %lu\n", hpwd);


    /* FOURTH, using polynomial f, generate instruction table, such that 
       all alpha and bravo values are valid */
    //INSERT FUNCTION HERE
//    status = genInstructionTable();
    long alphatable[numfeatures];
    long bravotable[numfeatures];
    long *alpha = alphatable;
    long *bravo = bravotable;
    computeAlpha(alpha, numfeatures, password, q, r);
    computeBravo(bravo, numfeatures, password, q, r);
    //Display Table:
    printf("Instruction Table:\n");
    for (i = 0; i < numfeatures; i++) {
        printf("{%d, %lu, %lu}\n",i+1,alpha[i],bravo[i]);
    }
    printf("End of Instruction Table.\n");


    /* FIFTH, initialize history file; if a history file is present already, 
       this is open decrypt, verify; if not present, this will create a new
       empty history file */
    history currentHistory;                     //Declare current history struct
    status = process_history(currentHistory);   //process history file

    printf("\nEnd of Initialization\n");

  return status;
}



//MAIN-----------------------------------------------
int main(int argc, char* argv[])
{
    printf("Beginning 'Server' execution.\n");

    //Verify correct arguments
    if (argc != 3) {
        printf("ERROR: Incorrect arguments\nformat:\n");
        printf("./program inputfile.txt outputfile.txt\n\nEXITING\n");
        return 0;
    }

    //Initialize Program Status
    int status = 0;

    //Declare buffers to hold password and features
    char password[200];
    char *pwd = (char *)&password;
    char features[200];
    char *feats = (char *)&features;

    //Run Program Initialization
    status = initProgram(argv);

    //Process input file to verify each password
//    status = processInput(argv, pwd, feats);
    
    return status;

}
