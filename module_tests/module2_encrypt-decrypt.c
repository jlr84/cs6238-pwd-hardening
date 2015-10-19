/* Program for validating individual module of project:
   Module 2: Encrypt and Decrypt
   --"Correctly encrypt and decrypt the constant-size
     history file with any Hpwd using symmetric cipher" */

//Compile using:
//   gcc module2_encrypt-decrypt.c -lcrypto -lm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "../crypt.c"

int main(int argc, char* argv[]) {

    printf("Beginning Encryption Test\n");
   
    printf("Enter password:\n");
    char buffer[30];
    fgets(buffer, 30, stdin);

    //converting input from char to long
    char *input, *endptr;
    input = buffer;
    long int hpwd;
    hpwd = strtol(input, &endptr, 36);
    printf("Hardened Password Inputted:\n%s\n",buffer);
    printf("Hdpw converted to long:\n%ld\n",hpwd);

    printf("Building sample history file buffer\n");
    //Compile output buffer
    char strsize[5];
    int hsize = 5;
    sprintf(strsize, "%d", hsize);
    unsigned char mybuf[1000] = "VALID\n";
    strcat(mybuf, strsize);
    strcat(mybuf, "\n");
    strcat(mybuf, "1,2,3,4,5,6,7,8,9,10,11,12,13,14\n");
    strcat(mybuf, "18,10,-2,20,10,4,2,8,6,17,23,20,27,7\n");
    strcat(mybuf, "20,14,2,18,10,5,5,7,11,19,27,18,29,7\n");
    strcat(mybuf, "16,11,7,20,9,4,1,13,4,21,26,16,27,16\n");
    strcat(mybuf, "14,13,12,11,10,9,8,7,6,5,4,3,2,1\n");
    strcat(mybuf, "END_OF_FILE\n\0");
    
    printf("Buffer created.\nSTART OF BUFFER:\n%sEND OF BUFFER\n",mybuf);
    printf("Padding to lenght = 1000\n");
    //Pad to length=1000
    unsigned char filler[]="aaa\0";
    int i;
    int length = strlen(mybuf);
    for (i = length; i < 1000; i = i + 1) {
        mybuf[i] = filler[1];
    }
    mybuf[999] = filler[3];
    
    printf("Buffer padded.\nSTART OF BUFFER:\n%sEND OF BUFFER\n",mybuf);

    //Create pointer to buffer just created
    unsigned char *mybufptr = (unsigned char*)&mybuf;

    printf("Opening output file: './test_encrypt'\n");
    //Open history file for writing
    FILE* hfile = fopen("./test_encrypt", "w+");
    
    printf("Encrypting buffer and writing to file\n");
    printf("[Calling 'encrypt_buf_to_file' with hpwd='%ld']\n",hpwd);
    //Encrypt buffer and write to file
    int status = encrypt_buf_to_file(mybufptr, hfile, hpwd);
    //Close history file
    fclose(hfile);
    printf("Buffer encrypted to file './test_encrypt'\n[Open this file to view encrypted buffer.]\n\n");

    printf("Beginning decryption...\n");
    printf("Re-enter password to decrypt:\n");
    char newbuffer[30];
    fgets(newbuffer, 30, stdin);

    //converting input from char to long
    char *newinput, *newendptr;
    newinput = newbuffer;
    long int newhpwd;
    newhpwd = strtol(newinput, &newendptr, 36);
    printf("Password Inputted to decrypt:\n%s\n",newbuffer);
    printf("Converted to long:\n%ld\n",newhpwd);

    printf("Opening Encrypted file './test_encrypt'; reading to buffer and decrypting.\n");
    printf("[Calling 'decrypt_file_to_buf' with hpwd='%ld']\n",newhpwd);
    //Open history file for reading 
    hfile = fopen("./test_encrypt", "r");
    unsigned char mybufin[5000];
    printf("Starting decryption buffer:>>>%s<<<\n",mybufin);
    status = decrypt_file_to_buf(hfile, mybufin, newhpwd);
    printf("Decrypted buffer:>>>%s<<<\n",mybufin);

    char compare[] = "VALID";
    if ( memcmp(mybufin, compare, 3) == 0 ) {
	printf("Decryption Successful\n");
    } else {
        printf("Decryption failure.\n");
    }
    printf("\nEND OF Decryption Test...Exiting\n");
    return 0;
}

