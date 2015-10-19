/* server.c
   Author: James Roberts
   CS6238 Project 1: Authenticated Encryption
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <gmp.h> //For big numbers!

#include "crypt.c" 
#include "tokenize.c" 
#include "utilities.h"
#include "server.h"
#include "random.h"
#include "random.c"
#include "polynomial.h"
#include "polynomial.c"
#include "lagrange.h"
#include "lagrange.c"
#include "pr-grp.h"
#include "pr-grp.c"


/* ERROR Codes, as structured:
111 = history file not found or not opened successfully;
222 = error during history file validation
333 = input file not found
*/


//Program Constants:
char const historyFolderLocation[] = "./history"; //ensure this matches below lines
char const historyFilePath[] = "./history/historyfile"; //ensure path matches above
char const alphaPath[] = "./history/alphatable"; //ensure path matches above
char const bravoPath[] = "./history/bravotable"; //ensure path matches above
int const historyFileSize = 1200;
int const ti = 10; // Given in project requirements
int const k = 2; // Given in project requirements


//Function Declarations:
void computeAlpha(Polynomial Poly, mpz_t* alpha, int numfeatures, char* password, mpz_t q, mpz_t r);
void computeBravo(Polynomial Poly, mpz_t* bravo, int numfeatures, char* password, mpz_t q, mpz_t r);
void computeGarbage(Polynomial Poly, mpz_t* table, int i, mpz_t q, mpz_t r);
void saveInstructionTable(int col, mpz_t * table, int size);



//Function for creating a new blank history file when it was not already present
int createFreshHistory(long hpwd) {
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
//    printf("\nBEGINNING ENCRYPTION; Starting with...\nInput Buffer: %s\n",mybufinptr);
    printf("Encrypting 'empty' history to file '%s'\n", historyFilePath);
    printf("Encrypt HPWD: %lu\n", hpwd);
    returncode = encrypt_buf_to_file(mybufinptr, historyFile, hpwd);
    fclose(historyFile);
    printf("Encryption Completed\n");
    printf("Fresh history file created.\n");
}


// Function processing history file stored in fixed location 
int process_history(history tempHistory, long hpwd) {
    int history_status = 0;
    FILE* inputfile = fopen(historyFilePath, "r");
    unsigned char decryptedtextbuffer[5000];
    unsigned char *decryptedtextptr = (unsigned char *)&decryptedtextbuffer;
//    history tempHistory;

    printf("\nProcessing History File:\n");

    if ( inputfile == 0) {
        history_status = 111;
        //fopen returns 0, the NULL pointer, on failure
        perror("\nHISTORY FILE MISSING");
        
        //Create new history file
        createFreshHistory(hpwd);
        inputfile = fopen(historyFilePath, "r");
    }
    printf("Decrypt HPWD: %ld\n", hpwd); 
    decrypt_file_to_buf(inputfile, decryptedtextptr, hpwd);
    char mybufin[5000];
        
    //Copy decryptedbuffer to mybufin[]
    strcpy(mybufin, decryptedtextptr);

//    printf("\nHistory File Decrypted; starting Buffer:\n%s\n\n", mybufin);

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
 
    //Since the file opened successfully, close it here
    fclose(inputfile);
        
    /* Compare Results to validate proper formatting; if invalid formatting, 
       assume corrupted file or incorrect decrypting */
    if ( strcmp(tempHistory.validation,"VALID") == 0 && strcmp(tempHistory.end,"END_OF_FILE") == 0 ) {
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

//Function for updating history file with values from current successful login
void update_history(history tempHistory, long hpwd, char* feats, int numfeatures, char* password, mpz_t q, mpz_t r ) {
    int status;


    /* First, Open history file, decrypt, and create new 
       history struct */
    FILE* inputfile = fopen(historyFilePath, "r");
    unsigned char decryptedtextbuffer[5000];
    unsigned char *decryptedtextptr = (unsigned char *)&decryptedtextbuffer;
    decrypt_file_to_buf(inputfile, decryptedtextptr, hpwd);
    char mybufin[5000];
    //Copy decryptedbuffer to mybufin[]
    strcpy(mybufin, decryptedtextptr);
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
    fclose(inputfile);
    // Compare Results to validate formatting 
    if ( strcmp(tempHistory.validation,"VALID") == 0 && strcmp(tempHistory.end,"END_OF_FILE") == 0 ) {
        printf("\nHistory File re-validated... \n");
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
    //Generate Base Instruction Table
    mpz_t atable[numfeatures];
    mpz_t btable[numfeatures];
    int j;
    for ( j = 0; j < numfeatures; j++) {
        mpz_init(atable[j]);
        mpz_init(btable[j]);
    }
    printf("Calculating new Alpha/Bravo Tables\n");
    computeAlpha(Poly[0], atable, numfeatures, password, q, r);
    computeBravo(Poly[0], btable, numfeatures, password, q, r);
    printf("Base tables computed.\n");


    /* Third, Update history file data; this will update
       slightly different depending on if we have h (h=5) 
       successful login attempts */
    printf("Updating history\n"); 
    
    //Open history file for writing
    FILE* hfile = fopen(historyFilePath, "w+");

    //convert string size to int
    int hsize = tempHistory.size[0] - '0';
    char strsize[5];
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
    unsigned char filler[]="aaa\0";
    int i; 
    int length = strlen(mybuf);
    for (i = length; i < 1000; i = i + 1) {
        mybuf[i] = filler[1];
    }
    mybuf[999] = filler[3];
    //Create pointer to buffer just created
    unsigned char *mybufptr = (unsigned char*)&mybuf;
    //Encrypt buffer and write to file
    status = encrypt_buf_to_file(mybufptr, hfile, newhpwd);
    //Close history file
    fclose(hfile);

     
    /* Fourth, if we have 5 previous successful logins, then
       we need to calculate mean/standard deviation and do 
       additional processing of instruction table */
    //Create integer array for each feature vector
    if ( hsize == 5 ) {
        printf("Features: \n%s%s\n%s\n%s\n%s\n", feats, tempHistory.line1, tempHistory.line2, tempHistory.line3, tempHistory.line4);
        int features1[numfeatures];
        int features2[numfeatures];
        int features3[numfeatures];
        int features4[numfeatures];
        int features5[numfeatures];
        //Store all features as integers 
        int num;
        num = str_to_ints(feats, features1);
        num = str_to_ints(tempHistory.line1, features2);
        num = str_to_ints(tempHistory.line2, features3);
        num = str_to_ints(tempHistory.line3, features4);
        num = str_to_ints(tempHistory.line4, features5);
/*        printf("Features Stored as Integers.\n");
        printf("01:|");
        for ( j = 0; j < numfeatures; j++) {
            printf(" %d |",features1[j]);
        }
        printf("\n02:|");
        for ( j = 0; j < numfeatures; j++) {
            printf(" %d |",features2[j]);
        }
        printf("\n03:|");
        for ( j = 0; j < numfeatures; j++) {
            printf(" %d |",features3[j]);
        }
        printf("\n04:|");
        for ( j = 0; j < numfeatures; j++) {
            printf(" %d |",features4[j]);
        }
        printf("\n05:|");
        for ( j = 0; j < numfeatures; j++) {
            printf(" %d |",features5[j]);
        }
*/
        //For each feature, compute mean and standard deviation
        float sdevs[numfeatures];
        float means[numfeatures];
        calculate_sdev_mean(features1, features2, features3, features4, features5, sdevs, means, numfeatures);
        printf("\nStandard Deviation and Mean Calculated.\n");
        for ( j = 0; j < numfeatures; j++ ) {
            printf("%2d SD: %.2f |MN: %.2f\n",j+1,sdevs[j],means[j]);
        }
	
        /* With Standard Deviation and mean computed, we will 
           now update the instruction tables based on the 
           specified constraints (see Monrose, et al., pg 6) */
        int gar = 0;
        //Change values to "garbage" based upon constraints
        for ( j = 0; j < numfeatures; j++) {
            //IF constraints are met, compute garbage for alpha or bravo
            if ( abs( means[j] - ti ) > (k*sdevs[j]) ) {
	        //IF mean < ti put garbage in bravo
	        if ( means[j] < ti ) {
	            computeGarbage(Poly[0], btable, j, q, r);
	            gar = gar + 1;
	        } else {  
	            //ELSE put garbage in alpha
	            computeGarbage(Poly[0], atable, j, q, r);
	            gar = gar + 1;
	        }
	    } 
	    //ELSE, leave valid computations in both alpha and bravo
        }
        printf("Tables updated with _%d_ garbage value(s).\n", gar);
    }
    //Display Table:
    printf("Instruction Table:\n");
    for (j = 0; j < numfeatures; j++) {
        gmp_printf("{%d, %Zd, %Zd}\n",j+1,atable[j],btable[j]);
        if ( j == 0 ) {
            printf("...%d to %d truncated for easier reading...\n",j+2,numfeatures-1);
            j = numfeatures - 2;
        }
    }
    printf("End of Instruction Table.\n");

    //Save "encrypted" instruction tables to file    
    saveInstructionTable(1, atable, numfeatures);
    saveInstructionTable(2, btable, numfeatures);
    printf("Updated Alpha-Bravo Tables Written to file\n");

    //Run "ProcessHistory" to verify saved history before exiting:
    status = process_history(tempHistory, newhpwd);
}


//Function for calculating all alpha column values as valid
void computeAlpha(Polynomial Poly, mpz_t* alpha, int numfeatures, char* password, mpz_t q, mpz_t r) {

    int i;
    mpz_t result, gr_result, pr_result, sum;
    mpz_init(result);
    mpz_init(gr_result);
    mpz_init(pr_result);
    mpz_init(sum);
  
    for (i = 1; i <= numfeatures; i++) {
        PR(pr_result, r, i*2);
//        gmp_printf("Pr(2i); i=%d: %Zd\n", i, pr_result); 
        AddPolynomial(Poly, pr_result, sum);
//        gmp_printf("Sum; i=%d: %Zd\n",i,sum);
        GRP(gr_result, r, password, i*2);
        mpz_mod(result, gr_result, q);
//        gmp_printf("Modresult[%d]: %Zd\n",i,result);
        mpz_add(alpha[i-1], sum, result);
//        gmp_printf("Alpha[%d]; i=%d: %Zd\n",i-1,i,alpha[i-1]);
	//Here is the equivalent of what we computed if using 'int' instead of gmp/mpz
	//alpha[i-1] = F(PR(r, i*2)) + ( GR(r, i*2, password) % q );
    }
    printf("Alpha Column initialized.\n");
}

//Function for calculating all alpha column values as valid
void computeBravo(Polynomial Poly, mpz_t* bravo, int numfeatures, char* password, mpz_t q, mpz_t r) {

    int i;
    mpz_t result, gr_result, pr_result, sum;
    mpz_init(result);
    mpz_init(gr_result);
    mpz_init(pr_result);
    mpz_init(sum);

    for (i = 1; i <= numfeatures; i++) {
        PR(pr_result, r, ((i*2)+1));
//	gmp_printf("Pr(2i); i=%d: %Zd\n", i, pr_result);
        AddPolynomial(Poly, pr_result, sum);
//        gmp_printf("Sum; i=%d: %Zd\n",i,sum);
        GRP(gr_result, r, password, ((i*2)+1));
        mpz_mod(result, gr_result, q);
//        gmp_printf("Modresult[%d]: %Zd\n",i,result);
        mpz_add(bravo[i-1], sum, result);
//        gmp_printf("Bravo[%d]; i=%d: %Zd\n",i-1,i,bravo[i-1]);
	//Here is the equivalent of what we computed if using 'int' instead of gmp/mpz
        //bravo[i-1] = PR(r, i*2+1) + ( GR(r, i*2+1, password) % q );
    }
    printf("Bravo Column initialized.\n");
}


//Function for calculating a garbage value for a specific feature
void computeGarbage(Polynomial Poly, mpz_t* table, int i, mpz_t q, mpz_t r) {

    /* This function uses a similar format to "computeAlpha" 
       and "computeBravo" so the values appear legitimate; 
       but this computes using an incorrect/garbage password
       and wrong values for input into the PR and GRP 
       functions; e.g., 3i+1 and i instead of 2i or 2i+1 */
    char wrongPassword[] = "ThisIsGarbage";
    mpz_t result, gr_result, pr_result, sum;
    mpz_init(result);
    mpz_init(gr_result);
    mpz_init(pr_result);
    mpz_init(sum);

    PR(pr_result, r, ((i*3)+1));
    AddPolynomial(Poly, pr_result, sum);
    GRP(gr_result, r, wrongPassword, i);
    mpz_mod(result, gr_result, q);
    mpz_add(table[i], sum, result);
//        gmp_printf("Bravo[%d]; i=%d: %Zd\n",i-1,i,bravo[i-1]);
}


//Function to decrypt alpha table; returns x table and y table
void decryptAlpha(mpz_t* xtable, mpz_t* ytable, mpz_t* alpha, int numfeatures, char* password, mpz_t q, mpz_t r) {
    int i;
    mpz_t pr_result, gr_result, mod_result, result;
    mpz_init(pr_result);
    mpz_init(gr_result);
    mpz_init(mod_result);
    mpz_init(result);
    for (i = 1; i <= numfeatures; i++) {
        //Calculte x value; "Pr(2i)"
        PR(xtable[i-1], r, i*2);
//        gmp_printf("Pr(2i); i=%d: %Zd\n", i, xtable[i-1]);
        //Calculate y value; "alpha(ai) - Gr,pwd(2i) mod q"
	GRP(gr_result, r, password, i*2);
        mpz_mod(mod_result, gr_result, q);
//        gmp_printf("Modresult[%d]: %Zd\n",i,mod_result);
//        gmp_printf("Alpha2; i=%d: %Zd\n",i,alpha[i-1]);
        mpz_sub(ytable[i-1], alpha[i-1], mod_result );
//        gmp_printf("Ytable; i=%d: %Zd\n",i,ytable[i-1]);
    }
}

//Function to decrypt bravo table
void decryptBravo(mpz_t* xtable, mpz_t* ytable, mpz_t* bravo, int numfeatures, char* password, mpz_t q, mpz_t r) {
    int i;
    mpz_t pr_result, gr_result, mod_result, result;
    mpz_init(pr_result);
    mpz_init(gr_result);
    mpz_init(mod_result);
    mpz_init(result);
    for (i = 1; i <= numfeatures; i++) {
        //Calculte x value; "Pr(2i+1)"
        PR(xtable[i-1], r, ((i*2)+1));
//        gmp_printf("Pr(2i+1); i=%d: %Zd\n", i, xtable[i-1]);
        //Calculate y value; "alpha(ai) - Gr,pwd(2i+1) mod q"
        GRP(gr_result, r, password, ((i*2)+1));
        mpz_mod(mod_result, gr_result, q);
//        gmp_printf("Modresult[%d]: %Zd\n",i,mod_result);
//        gmp_printf("Bravo2; i=%d: %Zd\n",i,bravo[i-1]);
        mpz_sub(ytable[i-1], bravo[i-1], mod_result );
//        gmp_printf("Ytable; i=%d: %Zd\n",i,ytable[i-1]);
    }
}


//Function to decrypt a mixed table; returns x table and y table
void decryptTable(mpz_t* xtable, mpz_t* ytable, mpz_t* ctable, int* map, int numfeatures, char* password, mpz_t q, mpz_t r) {
    int i;
    mpz_t pr_result, gr_result, mod_result, result;
    mpz_init(pr_result);
    mpz_init(gr_result);
    mpz_init(mod_result);
    mpz_init(result);

    
    for (i = 1; i <= numfeatures; i++) {
	if (map[i-1] == 1) {
            //Calculte x value; "Pr(2i)"
            PR(xtable[i-1], r, i*2);
//          gmp_printf("Pr(2i); i=%d: %Zd\n", i, xtable[i-1]);
            //Calculate y value; "alpha(ai) - Gr,pwd(2i) mod q"
            GRP(gr_result, r, password, i*2);
            mpz_mod(mod_result, gr_result, q);
//          gmp_printf("Modresult[%d]: %Zd\n",i,mod_result);
            mpz_sub(ytable[i-1], ctable[i-1], mod_result );
//          gmp_printf("Ytable; i=%d: %Zd\n",i,ytable[i-1]);
        } else { //If map[i-1] == 2, then:
            //Calculte x value; "Pr(2i+1)"
            PR(xtable[i-1], r, ((i*2)+1));
//          gmp_printf("Pr(2i+1); i=%d: %Zd\n", i, xtable[i-1]);
            //Calculate y value; "alpha(ai) - Gr,pwd(2i+1) mod q"
            GRP(gr_result, r, password, ((i*2)+1));
            mpz_mod(mod_result, gr_result, q);
//          gmp_printf("Modresult[%d]: %Zd\n",i,mod_result);
            mpz_sub(ytable[i-1], ctable[i-1], mod_result );
//          gmp_printf("Ytable; i=%d: %Zd\n",i,ytable[i-1]);
	}
    }
}


/* Saves instruction table to disk at destination listed in program
   constants section above; 1 for alpha column; 2 for bravo column */
void saveInstructionTable(int col, mpz_t * table, int size) {
    //Determine which file to write to (1=alpha; 2 =bravo)
    char *name;
    if (col == 1 ) { name=(char *)alphaPath; }
    else if (col == 2) { name=(char *)bravoPath; }
    else { 
	printf("ERROR: Instruction Table not saved\n"); 
	return; 
    }

    //Open file for writing
    FILE *f;
    f = fopen(name, "w+");
    int i;
    //For each stored instruction table entry, write to file
    for ( i = 0; i < size; i++ ) {
	mpz_out_str(f, 10, table[i]);
        //Add newline character after each entry 
        fputs("\n",f);
    }
    fclose(f);
}

/* Open instruction table from file and read into mpz_t table; 
   read based on constants section above; 1 for alpha column; 
   2 for bravo column */
void readInstructionTable(int col, mpz_t * table, int size) {
    char *name;
    if (col == 1 ) { name=(char *)alphaPath; }
    else if (col == 2) { name=(char *)bravoPath; }
    else {
        printf("ERROR: Instruction Table not opened.\n");
        return;
    }
    
    //Create  buffer for reading in each line
    unsigned char buff[800];
    
    //Open file for reading
    FILE *f;
    f = fopen(name, "r");
    printf("File %s opened\n",name);

    if (!f) {
 	printf("Input file not found\n");
    } else {
        int i = 0;
        //Read in file to table[i]
	for ( i = 0; i < size; i++ ) {
	    mpz_inp_str(table[i], f, 10);
	}
    }
    fclose(f);
}


//Verifies each password/feature pair; returns 1 if good password; returns 0 if bad password
int verifyPassword(char* pwd, char* feats, mpz_t q, mpz_t r) {
    
    int status = 0;
    char password[200];
    int features[127]; 
    int numfeatures;
    
    /* Step 1: Initial processing of inputted password and
       features from input file */
    //Copy pwd string to interal buffer, removing 'newline' character
    int pwdlen = copy_pwd(password, pwd);
    //Display password inputted and length
    printf("Password:>>>%s<<<\nLength: %d\n", password, pwdlen);
    //Display feature string
    printf("Features: %s", feats);
    //Change feature string to integers and display
    numfeatures = str_to_ints(feats, features);
    printf("Number of Features: %d\n",numfeatures);
    int i;
    printf("Features Stored as Integers:\n|");
    for ( i = 0; i < numfeatures; i++) {
        printf(" %d |",features[i]);
    }

    /* Step 2: Read instruction table in from file; prepare 
       for processing with inputted feature vector */
    mpz_t tableA[numfeatures]; //Will hold alpha table
    mpz_t tableB[numfeatures]; //Will hold bravo table
    mpz_t currentTable[numfeatures]; //Will hold table for current logon
    for (i = 0; i < numfeatures; i++) {
	mpz_init(tableA[i]);
	mpz_init(tableB[i]);
	mpz_init(currentTable[i]);
    }
    printf("\nReading tables from file...\n");
    readInstructionTable(1, tableA, numfeatures);
    readInstructionTable(2, tableB, numfeatures);
    printf("End of read.\n");
/*    printf("Instruction Table:\n");
    for (i = 0; i < numfeatures; i++ ) {
        gmp_printf("{%d, %Zd, %Zd}\n",i+1,tableA[i],tableB[i]);
    }
*/    

    /* Step 3: Build instruction table based on values found
       in feature vectors inputted */
    printf("Building instruction table...\n");
    /* map will store where we retrieved each feature value; 
       1 depicts an 'alpha' value; 2 depicts a 'bravo' value */
    int map[numfeatures];
    for (i = 0; i < numfeatures; i++) {
	if ( features[i] < ti ) {
	    mpz_set(currentTable[i], tableA[i]);
	    map[i] = 1;
	    printf("Table[%d]: Alpha|%d\n",i+1,map[i]);
	} else {
	    mpz_set(currentTable[i], tableB[i]);
            map[i] = 2;  
	    printf("Table[%d]: Bravo|%d\n",i+1,map[i]);
	}
    }
    printf("Table Constructed\n");

    /* Step 4: Decrypt current instruction table using r and 
       password; then use lagrange to compute hpwd */
    printf("Decrypting Table...\n");
    mpz_t xtableC[numfeatures];
    mpz_t ytableC[numfeatures];
    for ( i = 0; i < numfeatures; i++ ) {
        mpz_init(xtableC[i]);
	mpz_init(ytableC[i]);
    }
    decryptTable(xtableC, ytableC, currentTable, map, numfeatures, password, q, r);
    //Change mpz x and y tables to mpf
    mpf_t computedHpwd;
    mpf_init(computedHpwd);
    long computedHpwd2;
//    printf("Changing mpz to mpf\n");
    mpf_t xtableCf[numfeatures];
    mpf_t ytableCf[numfeatures];
    for ( i = 0; i < numfeatures; i++ ) {
        mpf_init(xtableCf[i]);
        mpf_init(ytableCf[i]);
        mpf_set_z(xtableCf[i], xtableC[i]);
        mpf_set_z(ytableCf[i], ytableC[i]);
    }
    //Now, compute hpwd using lagrange:
    printf("Computing Lagrange...\n");
    Lagrange(computedHpwd, numfeatures, xtableCf, ytableCf);
    computedHpwd2 = Xround(computedHpwd);
    printf("Computed Hpwd: %ld\n",computedHpwd2);

    /* Step 5: Using computed Hpwd to attempt to decrypt history
       file; unsuccesful decryption means either the password or 
       feature vectors (e.g., the computed hpwd) were incorrect */
    //Declare current history struct
    history currentHistory; 
    //Process history file/attempt to decrypt (222 = failed)
    status = process_history(currentHistory, computedHpwd2);
    if (status == 222) { 
        printf("Attempting 'simple error correction'...\n");
	// ADD LOGIC for Simple Error Correction Here
    } else if (status == 1) {
	printf("Password Validated\n");
        //Update History file and save to disk
        update_history(currentHistory, computedHpwd2, feats, numfeatures, password, q, r); 
        
    } else {
	printf("Unknown Error\n");
    }

    return status;
}


//Processes input file from beginning to end; calls verifyPassword() for each password/feature pair
int processInput(char* argv[], char* pwd, char* feats, mpz_t q, mpz_t r) {

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
//                printf("ODD: %s\n",buff);
                //Password i has now been copied out.

            } else {
                //Else, i.e., if i is even do this:
                strcpy(feats, buff);
//                printf("EVEN: %s\n", buff);
                //Features i have now been copied out; time to verify
		printf("\nProcessing Password/Feature #%d...\n",i/2);
                verified = verifyPassword(pwd, feats, q, r);
                printf("FINAL STATUS: %d\n",verified);
                if (verified == 1) {
                    fputs("1\n", output);
                } else {
                    fputs("0\n", output);
                }
                printf("END OF Password/Feature #%d verification\n\n", i/2);
            }
            i = i + 1;

	    //TEMPORARY TRUNCATION 
//	    if ( i == 14) { fclose(f); fclose(output); printf("ExitingEarly\n"); return status; } 

        }
        printf("End of input file.\n");
    }
    fclose(f);
    fclose(output);
    printf("Function readInput() complete.\n");
    return status;
}


//Runs program initialization sequence
int initProgram(char* argv[], mpz_t q, mpz_t r) {
    int status = 0;
    /////////////////////////////
    //INITIALIZATION
    /////////////////////////////
    printf("Beginning Initialization.\n");

    //Make history directory if not already present on system:
    struct stat st ={0};
    if (stat(historyFolderLocation, &st) == -1) {
        mkdir(historyFolderLocation, 0700);
    }

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
//    mpz_t q;
    RandomPrime(q);   
    gmp_printf("\nq: %Zd\n", q);
//    mpz_t r;
    RandomNumber(r);
    gmp_printf("r: %Zd\n", r);


    /* THIRD, determine Maximum number of distinguising features, m,
       then select polynomial f of degree m-1 */
    printf("Distinguishing Features: %d\n", numfeatures);
    printf("Selecting Polynomial of degree: %d\n", numfeatures-1);
    Polynomial Poly[1];  	//Init poly struct
    int realdegree = numfeatures -1;
    InputPolynomial(&Poly[0], realdegree);
    PrintPolynomial(Poly[0]);	


    //Select polynomial f of degree m-1
    //f(0) will give us our initial hpwd
    long hpwd = Poly[0].CoeffArray[0];
    printf("Random hpwd: %ld\n", hpwd);


    /* FOURTH, using polynomial f, generate instruction table, such that 
       all alpha and bravo values are valid */
    mpz_t alphatable[numfeatures];
    mpz_t bravotable[numfeatures];
    for ( i = 0; i < numfeatures; i++) {
        mpz_init(alphatable[i]);
	mpz_init(bravotable[i]);
    }
    printf("Computing Alpha and Bravo Tables\n");
    computeAlpha(Poly[0], alphatable, numfeatures, password, q, r);
    computeBravo(Poly[0], bravotable, numfeatures, password, q, r);
    //Display Table:
    printf("Instruction Table:\n");
    for (i = 0; i < numfeatures; i++) {
        gmp_printf("{%d, %Zd, %Zd}\n",i+1,alphatable[i],bravotable[i]);
        if ( i == 0 ) { 
	    printf("...%d to %d truncated for easier reading...\n",i+2,numfeatures-1);
            i = numfeatures - 2;
	}
    }
    printf("End of Instruction Table.\n\n");

    //Save "encrypted" instruction tables to file    
    saveInstructionTable(1, alphatable, numfeatures);
    saveInstructionTable(2, bravotable, numfeatures);
    printf("Alpha-Bravo Tables Written to file\n");
    

    /* FIFTH, verify instruction table; we will do this by "decrypting" 
       the alpha and bravo tables and using the Lagrange function for each;
       should produce an X(0) value that matches the hpwd */
    printf("Decrypting Alpha Table\n");
    mpz_t xtablea[numfeatures];
    mpz_t ytablea[numfeatures];
    for ( i = 0; i < numfeatures; i++ ) {
        mpz_init(xtablea[i]);
        mpz_init(ytablea[i]);
    }
//    printf("Password: %s; NumFeatures: %d\n",password, numfeatures);
//    gmp_printf("Q: %Zd\nR: %Zd\n",q,r);
    decryptAlpha(xtablea, ytablea, alphatable, numfeatures, password, q, r);
    mpf_t computedHpwdA;
    mpf_init(computedHpwdA);
    long computedHpwdA2;
    //Change mpz x and y tables to mpf
//    printf("Changing mpz to mpf\n");
    mpf_t xtableaf[numfeatures];
    mpf_t ytableaf[numfeatures];
    for ( i = 0; i < numfeatures; i++ ) {
	mpf_init(xtableaf[i]);
        mpf_init(ytableaf[i]);
        mpf_set_z(xtableaf[i], xtablea[i]);
        mpf_set_z(ytableaf[i], ytablea[i]);
//        gmp_printf("xtablez[%d]: %Zd\n xtablef[%d]: %Ff\n",i,xtable[i],i,xtablef[i]);
//        gmp_printf("ytablez[%d]: %Zd\n ytablef[%d]: %Ff\n",i,ytable[i],i,ytablef[i]);

    }
    printf("Computing Alpha Lagrange...\n");
    Lagrange(computedHpwdA, numfeatures, xtableaf, ytableaf);
//    gmp_printf("Computed Hpwd-Alpha: %Ff\n", computedHpwdA);
    printf("Original Hpwd: %ld\n", hpwd);
    computedHpwdA2 = Xround(computedHpwdA); 
    printf("Computed Hpwd-Alpha: %ld\n",computedHpwdA2);
    if (computedHpwdA2 == hpwd) { printf("MATCH\n"); }
    else { printf("FAIL\n"); }

    printf("Decrypting Bravo Table\n");
    mpz_t xtableb[numfeatures];
    mpz_t ytableb[numfeatures];
    for ( i = 0; i < numfeatures; i++ ) {
        mpz_init(xtableb[i]);
        mpz_init(ytableb[i]);
    }
//    printf("Password: %s; NumFeatures: %d\n",password,numfeatures);
//    gmp_printf("Q: %Zd\nR: %Zd\n",q,r);
    decryptBravo(xtableb, ytableb, bravotable, numfeatures, password, q, r);
    //Change mpz x and y tables to mpf
    mpf_t computedHpwdB;
    mpf_init(computedHpwdB);
    long computedHpwdB2;
//    printf("Changing mpz to mpf\n");
    mpf_t xtablebf[numfeatures];
    mpf_t ytablebf[numfeatures];
    for ( i = 0; i < numfeatures; i++ ) {
        mpf_init(xtablebf[i]);
        mpf_init(ytablebf[i]);
        mpf_set_z(xtablebf[i], xtableb[i]);
        mpf_set_z(ytablebf[i], ytableb[i]);
//        gmp_printf("xtablez[%d]: %Zd\n xtablef[%d]: %Ff\n",i,xtable[i],i,xtablef[i]);
//        gmp_printf("ytablez[%d]: %Zd\n ytablef[%d]: %Ff\n",i,ytable[i],i,ytablef[i]);
    }

    printf("Computing Bravo Lagrange...\n");
    Lagrange(computedHpwdB, numfeatures, xtablebf, ytablebf);
//    gmp_printf("Computed Hpwd-Bravo: %Ff\n", computedHpwdB);
    printf("Original Hpwd: %ld\n", hpwd);
    computedHpwdB2 = Xround(computedHpwdB); 
    printf("Computed Hpwd-Bravo: %ld\n",computedHpwdB2);
    if (computedHpwdB2 == hpwd) { printf("MATCH\n"); }    
    else { printf("FAIL\n"); }

    if (computedHpwdA2 == hpwd && computedHpwdB2 == hpwd) {
	printf("Instruction Table Verified Correct\n\n");
    }

    /* SIXTH, initialize history file; this will create a new
       empty history file then open and verify it to ensure
       encryption / decryption works */
    printf("Initializing History File:\n");
    //Declare current history struct
    history currentHistory;
    //Create new history file
    createFreshHistory(hpwd);
    //Process history file to verify encryption / decryption is working
    status = process_history(currentHistory, hpwd);

    printf("\nEnd of Initialization\n\n");


    /* EXTRA CREDIT, this reads in the stored alpha and bravo
       instruction tables and verifies they match the values
       already stored in memory. */
    mpz_t vtablea[numfeatures];
    mpz_t vtableb[numfeatures];
    for ( i = 0; i < numfeatures; i++ ) {
        mpz_init(vtablea[i]);
        mpz_init(vtableb[i]);
    }
    printf("Reading tables from file...\n");
    readInstructionTable(1, vtablea, numfeatures);
    readInstructionTable(2, vtableb, numfeatures);
    printf("End of read.\n");
/*    printf("Verification Instruction Table:\n");
    for (i = 0; i < numfeatures; i++) {
        gmp_printf("{%d, %Zd, %Zd}\n",i+1,vtablea[i],vtableb[i]);
    }
    printf("End of Verified Instruction Table.\n\n");
*/  int verified = 0;
    int finalverification = 0;
    for ( i = 0; i < numfeatures; i++ ) {
        verified = mpz_cmp(alphatable[i], vtablea[i]);
        if ( verified != 0 ) { finalverification = 888; }
        verified = mpz_cmp(bravotable[i], vtableb[i]);
        if ( verified != 0 ) { finalverification = 888; }
    }
    if ( finalverification == 0 ) { 
	printf("Instruction Table file SUCCESSFULLY read and VERIFIED.\n"); 
    } else { printf("Instruction Table FAILED validation\n"); } 

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
    mpf_set_default_prec(3000);

    //Declare buffers to hold password and features
    mpz_t q;
    mpz_t r;
    mpz_init(q);
    mpz_init(r);
    char password[200];
    char *pwd = (char *)&password;
    char features[200];
    char *feats = (char *)&features;

    //Run Program Initialization
    status = initProgram(argv, q, r);

    //Process input file to verify each password
    printf("\nProcessing of user input [file] now.\n");
    status = processInput(argv, pwd, feats, q, r);
    
    return status;

}
