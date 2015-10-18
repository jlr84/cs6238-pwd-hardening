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
char const historyFolderLocation[] = "./history"; // ensure path here is also on below line
char const historyFilePath[] = "./history/historyfile"; // ensure path matches above line
char const alphaPath[] = "./history/alphatable"; 
char const bravoPath[] = "./history/bravotable";
char const validationString[] = "VALID\n";
char const endFileString[] = "END_OF_FILE\n";
int const historyFileSize = 1200;


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
int process_history(history currentHistory, long hpwd) {
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
        createFreshHistory(hpwd);
        inputfile = fopen(historyFilePath, "r");
    }
    printf("Decrypt HPWD: %lu\n", hpwd); 
    decrypt_file_to_buf(inputfile, decryptedtextptr, hpwd);
    char mybufin[5000];
        
    //Copy decryptedbuffer to mybufin[]
    strcpy(mybufin, decryptedtextptr);

//    printf("\nHistory File Decrypted; starting Buffer:\n%s\n\n", mybufin);

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
};

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
};


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
int verifyPassword(char* pwd, char* feats) {
    
    int status = 0;

    history currentHistory; 			//Declare current history struct
//    status = process_history(currentHistory, hpwd);	//process history file

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
    mpz_t q;
    RandomPrime(q);   
    gmp_printf("\nq: %Zd\n", q);
    mpz_t r;
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
        if ( i == 1 ) { 
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
    char password[200];
    char *pwd = (char *)&password;
    char features[200];
    char *feats = (char *)&features;

    //Run Program Initialization
    status = initProgram(argv);

    //Process input file to verify each password
    //status = processInput(argv, pwd, feats);
    
    return status;

}
