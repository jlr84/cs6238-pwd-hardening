/* Program for validating individual module of project:
   Module 4: Update History File
   --"Correctly update the history file" */

//Compile using:
//   gcc module4_update-history-file.c -lcrypto -lm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "../crypt.c"
#include <gmp.h> //For big numbers!

#include "../server.h"
#include "../tokenize.c"
#include "../utilities.h"
#include "../random.h"
#include "../random.c"
#include "../polynomial.h"
#include "../polynomial.c"
#include "../pr-grp.h"
#include "../pr-grp.c"


int process_history2(history tempHistory, long hpwd);


int main(int argc, char* argv[]) {
    
    //Initial Values Required for Testing
    mpz_t r, q;
    mpz_init(r);
    mpz_init(q);
    RandomPrime(q);
    long int hpwd = 2345678;
    history tempHistory;
    char feats[] = "14,13,12,11,10,9,8,7,6,5,4,3,2,1\n";
    int numfeatures = 14;
    char password[] = "TestCorrectPwd";

    printf("\nBeginning History File Test\n");
    printf("Note: The feature vector used for all updates is the following:\n%s\n",feats);

    printf("Building sample history file for testing...\n");    
    //Compile output buffer
    char strsize[5];
    int hsize = 3;
    sprintf(strsize, "%d", hsize);
    unsigned char mytestbuf[1000] = "VALID\n";
    strcat(mytestbuf, strsize);
    strcat(mytestbuf, "\n");
    strcat(mytestbuf, "1,2,3,4,5,6,7,8,9,10,11,12,13,14\n");
    strcat(mytestbuf, "18,10,-2,20,10,4,2,8,6,17,23,20,27,7\n");
    strcat(mytestbuf, "20,14,2,18,10,5,5,7,11,19,27,18,29,7\n");
    strcat(mytestbuf, "NA\n");
    strcat(mytestbuf, "NA\n");
    strcat(mytestbuf, "END_OF_FILE\n\0");
    printf("Buffer created.\nSTART OF BUFFER:\n%sEND OF BUFFER\n",mytestbuf);
    printf("Padding to length = 1000\n");
    //Pad to length=1000
    unsigned char filler[]="aaa\0";
    int i;
    int length = strlen(mytestbuf);
    for (i = length; i < 1000; i = i + 1) {
        mytestbuf[i] = filler[1];
    }
    mytestbuf[999] = filler[3];
    printf("Buffer padded.\n");
    //Create pointer to buffer just created
    unsigned char *mytestbufptr = (unsigned char*)&mytestbuf;
    printf("Opening output file: './test_history-update'\n");
    //Open history file for writing
    FILE* hfile = fopen("./test_history-update", "w+");
    printf("Encrypting buffer and writing to file\n");
    //Encrypt buffer and write to file
    int status = encrypt_buf_to_file(mytestbufptr, hfile, hpwd);
    //Close history file
    fclose(hfile);
    printf("Buffer encrypted to file './test_history-update'\n");

int j;
/* We are starting with h = 3 successful login attempts; by
   looping 4 times, we will see the history file update 
   multiples time, both above and below h = 5 */
for ( j = 1; j < 5; j++ ) {
    /* First, Open history file, decrypt, and create new 
       history struct */
    printf("\n\nPROCESSING UPDATE #%d\n\n",j);
    FILE* inputfile = fopen("./test_history-update", "r");
    unsigned char decryptedtextbuffer[5000];
    unsigned char *decryptedtextptr = (unsigned char *)&decryptedtextbuffer;
    decrypt_file_to_buf(inputfile, decryptedtextptr, hpwd);
    char mybufin[5000];
    //Copy decryptedbuffer to mybufin[]
    strcpy(mybufin, decryptedtextptr);

    int bufsize = strlen(mybufin);
    char compare[] = "VALID";
    if (bufsize > 0) {
        //If the buffer starts with "V", tokenize
        if ( memcmp(mybufin, compare, 1) == 0 ) {
            //Tokenize our input buffer
            char **tokens;
            tokens = str_split(mybufin, '\n');
            if (tokens) {
                tempHistory.validation = *(tokens + 0);
                tempHistory.size = *(tokens + 1);
                tempHistory.line1 = *(tokens + 2);
                tempHistory.line2 = *(tokens + 3);
                tempHistory.line3 = *(tokens + 4);
                tempHistory.line4 = *(tokens + 5);
                tempHistory.line5 = *(tokens + 6);
                tempHistory.end = *(tokens + 7);
            }
        }
    }
    //Close inputfile
    fclose(inputfile);

    //Compare results to validate formatting
    if (bufsize == 0 || memcmp(mybufin, compare, 1) != 0) {
       printf("\nHistory File Error, buffer not valid\n");
    } else if ( strcmp(tempHistory.validation,"VALID") == 0 && strcmp(tempHistory.end,"END_OF_FILE") == 0 ) {
        printf("\nHistory File re-validated...\nStarting History:\n");
        //Print to screen for validation/testing
        printf("VAL:%s\nSIZE:%s\nLN1:%s\nLN2:%s\nLN3:%s\nLN4:%s\nLN5:%s\nEND:%s\n\n",tempHistory.validation,tempHistory.size,tempHistory.line1,tempHistory.line2,tempHistory.line3,tempHistory.line4,tempHistory.line5,tempHistory.end);

    } else {
        printf("\nHistory File Error, or Invalid Decryption\n");
    }


    /* Second, pick a new r and polynomial, and calculate
       new hardened password */
    //Select new r
    RandomNumber(r);
    gmp_printf("New r selected: \n%Zd\n",r);
    //Select new polynomial
    printf("Creating new polynomial.\n");
    Polynomial Poly[1];
    int realdegree = numfeatures -1;
    InputPolynomial(&Poly[0], realdegree);
    PrintPolynomial(Poly[0]);
    long newhpwd = Poly[0].CoeffArray[0];
    printf("New Hpwd: %ld\n", newhpwd);

    /* Third, Update history file data; this will update
       slightly different depending on if we have h (h=5) 
       successful login attempts */
    printf("\nUpdating history now...\n\n");

    //Open history file for writing
    hfile = fopen("./test_history-update", "w+");

    //convert string size to int
    hsize = tempHistory.size[0] - '0';
    strsize[5];
//    printf("hsize: %d\n",hsize);

    //if size <=3, add one to size
    if ( hsize <= 3 ) {
        hsize = hsize + 1;
        //convert size to string for printing
        sprintf(strsize, "%d", hsize);
    } else {
        hsize = 5;
        sprintf(strsize, "%d", hsize);
    }
    //Compile output buffer
    unsigned char mybuf[1000] = "VALID\n";
    strcat(mybuf, strsize);
    strcat(mybuf, "\n");
    strcat(mybuf, feats);
    strcat(mybuf, tempHistory.line1);
    strcat(mybuf, "\n");
    strcat(mybuf, tempHistory.line2);
    strcat(mybuf, "\n");
    strcat(mybuf, tempHistory.line3);
    strcat(mybuf, "\n");
    strcat(mybuf, tempHistory.line4);
    strcat(mybuf, "\n");
    strcat(mybuf, "END_OF_FILE\n\0");
    //Pad to length=1000
    //filler[]="aaa\0";
    length = strlen(mybuf);
    for (i = length; i < 1000; i = i + 1) {
        mybuf[i] = filler[1];
    }
    mybuf[999] = filler[3];
    //Create pointer to buffer just created
    unsigned char *mybufptr = (unsigned char*)&mybuf;
    //Encrypt buffer and write to file
    printf("Encrypting and writing to file.\n");
    status = encrypt_buf_to_file(mybufptr, hfile, newhpwd);
    //Close history file
    fclose(hfile);
    printf("Update Complete.\n\nDecrypting to verify update.\n");

    //Run "ProcessHistory" to verify saved history before exiting:
    status = process_history2(tempHistory, newhpwd);

    hpwd = newhpwd;
    printf("\nEnd of update #%d\n",j);
}
    printf("\nEND OF History File Test...Exiting\n");
    return 0;
}

/* Function processing history file stored in fixed location
   NOTE: The only difference from the main server program is
   is the location of the history file being processed */
int process_history2(history tempHistory, long hpwd) {
    int history_status = 0;
    FILE* inputfile = fopen("./test_history-update", "r");
    unsigned char decryptedtextbuffer[5000];
    unsigned char *decryptedtextptr = (unsigned char *)&decryptedtextbuffer;
//    history tempHistory;

    printf("\nProcessing History File:\n");

    if ( inputfile == 0) {
        history_status = 111;
        //fopen returns 0, the NULL pointer, on failure
        perror("\nHISTORY FILE MISSING");

        //Create new history file
        //createFreshHistory(hpwd);
        //inputfile = fopen(historyFilePath, "r");
    }
    printf("Decrypt HPWD: %ld\n", hpwd);
    decrypt_file_to_buf(inputfile, decryptedtextptr, hpwd);
    char mybufin[5000];

    //Copy decryptedbuffer to mybufin[]
    strcpy(mybufin, decryptedtextptr);
//    printf("\nHistory File Decrypted; starting Buffer:\n%s\n\n", mybufin);
//    printf("Length of mybufin: %d\n",strlen(mybufin));
    int bufsize = strlen(mybufin);
    char compare[] = "VALID";
    if (bufsize > 0) {
        //If the buffer starts with "V", tokenize
        if ( memcmp(mybufin, compare, 1) == 0 ) {
            //Tokenize our input buffer
            char **tokens;
            tokens = str_split(mybufin, '\n');
            if (tokens) {
                tempHistory.validation = *(tokens + 0);
                tempHistory.size = *(tokens + 1);
                tempHistory.line1 = *(tokens + 2);
                tempHistory.line2 = *(tokens + 3);
                tempHistory.line3 = *(tokens + 4);
                tempHistory.line4 = *(tokens + 5);
                tempHistory.line5 = *(tokens + 6);
                tempHistory.end = *(tokens + 7);
            }
        }
    }
    //Since the file opened successfully, close it here
    fclose(inputfile);

    /* Compare Results to validate proper formatting; if invalid formatting, 
       assume corrupted file or incorrect decrypting */
    if (bufsize == 0 || memcmp(mybufin, compare, 1) != 0) {
       printf("\nHistory File Error, buffer not valid\n");
       history_status = 222;
    } else if ( strcmp(tempHistory.validation,"VALID") == 0 && strcmp(tempHistory.end,"END_OF_FILE") == 0 ) {
        printf("\nHistory File Validated Successfully:\n");
        history_status = 1;
        //Print to screen for validation/testing
        printf("VAL:%s\nSIZE:%s\nLN1:%s\nLN2:%s\nLN3:%s\nLN4:%s\nLN5:%s\nEND:%s\n",tempHistory.validation,tempHistory.size,tempHistory.line1,tempHistory.line2,tempHistory.line3,tempHistory.line4,tempHistory.line5,tempHistory.end);
    } else {
        printf("\nHistory File Error, or Invalid Decryption\n");
        history_status = 222;
    }
    return history_status;
}


