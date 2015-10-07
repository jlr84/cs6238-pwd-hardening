/* readinput.c
   Author: James Roberts
   CS6238 Project 1: Authenticated Encryption
   - Reads input file two lines at a time until end of file
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


int verifyPassword(char* pwd, char* feats) {

    return 1;

}

void readInput(char* argv[], char* pwd, char* feats) {

    //Open Input File
    FILE *f = fopen(argv[1], "r");
    //Open Output File
    FILE *output = fopen(argv[2], "w+");

    char buff[200];
    int status;

    if (!f) {
	printf("INPUT FILE NOT FOUND");
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
		status = verifyPassword(pwd, feats);
		printf("STATUS: %d\n",status);
		if (status == 1) {
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
}

//MAIN-----------------------------------------------
int main(int argc, char* argv[])
{

    //Verify correct arguments
    if (argc != 3) {
        printf("ERROR: Incorrect arguments\nformat:\n./program inputfile.txt outputfile.txt\n\nEXITING\n");
        return 0;
    }
    char password[200];
    char *pwd = (char *)&password;
    char features[200];
    char *feats = (char *)&features;

    readInput(argv, pwd, feats);

}



