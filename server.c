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

#include "crypt.c" //crypt_standalone.c is the same with 'main' added for testing
#include "tokenize.c" //tokenize_standalone.c is the same with 'main' added for testing
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
        PR(pr_result, r, i*2+1);
        AddPolynomial(Poly, pr_result, sum);
        GRP(gr_result, r, password, i*2+1);
        mpz_mod(result, gr_result, q);
        //gmp_printf("Modresult[%d]: %Zd\n",i,result);
        mpz_add(bravo[i-1], sum, result);
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
        //Calculte x value; "Pr(2i)"
        PR(xtable[i-1], r, i*2+1);
//        gmp_printf("Pr(2i); i=%d: %Zd\n", i, xtable[i-1]);
        //Calculate y value; "alpha(ai) - Gr,pwd(2i) mod q"
        GRP(gr_result, r, password, i*2+1);
        mpz_mod(mod_result, gr_result, q);
//        gmp_printf("Modresult[%d]: %Zd\n",i,mod_result);
//        gmp_printf("Bravo2; i=%d: %Zd\n",i,bravo[i-1]);
        mpz_sub(ytable[i-1], bravo[i-1], mod_result );
//        gmp_printf("Ytable; i=%d: %Zd\n",i,ytable[i-1]);
    }
}

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
    }
    printf("End of Instruction Table.\n");


    /* FIFTH, verifty instruction table; we will do this by "decrypting" 
       the alpha and bravo tables and using the Lagrange function for each
       produces and X(0) value that matches the hpwd */
    mpz_t xtable[numfeatures];
    mpz_t ytable[numfeatures];
    for ( i = 0; i < numfeatures; i++ ) {
        mpz_init(xtable[i]);
        mpz_init(ytable[i]);
    }
    printf("Decrypting Alpha Table\n");
    decryptAlpha(xtable, ytable, alphatable, numfeatures, password, q, r);
    mpf_t computedHpwd;
    mpf_init(computedHpwd);
    long computedHpwd2;
    //Change mpz x and y tables to mpf
    printf("Changing mpz to mpf\n");
    mpf_t xtablef[numfeatures];
    mpf_t ytablef[numfeatures];
    for ( i = 0; i < numfeatures; i++ ) {
	mpf_init(xtablef[i]);
        mpf_init(ytablef[i]);
        mpf_set_z(xtablef[i], xtable[i]);
        mpf_set_z(ytablef[i], ytable[i]);
//        gmp_printf("xtablez[%d]: %Zd\n xtablef[%d]: %Ff\n",i,xtable[i],i,xtablef[i]);
//        gmp_printf("ytablez[%d]: %Zd\n ytablef[%d]: %Ff\n",i,ytable[i],i,ytablef[i]);

    }
    printf("Computing Lagrange...\n");
    Lagrange(computedHpwd, numfeatures, xtablef, ytablef);
    gmp_printf("Computed Hpwd-Alpha: %Ff\n", computedHpwd);
    printf("Original Hpwd: %ld\n", hpwd);
    computedHpwd2 = Xround(computedHpwd); 
    printf("ComputedLong Hpwd-Alpha: %ld\n",computedHpwd2);
    if (computedHpwd2 == hpwd) { printf("MATCH\n"); }

    printf("Decrypting Bravo Table\n");
    decryptBravo(xtable, ytable, bravotable, numfeatures, password, q, r);
    //Change mpz x and y tables to mpf
    printf("Changing mpz to mpf\n");
    for ( i = 0; i < numfeatures; i++ ) {
        mpf_set_z(xtablef[i], xtable[i]);
        mpf_set_z(ytablef[i], ytable[i]);
    }
    printf("Computing Lagrange...\n");
    Lagrange(computedHpwd, numfeatures, xtablef, ytablef);
    gmp_printf("Computed Hpwd-Bravo: %Ff\n", computedHpwd);
    printf("Original Hpwd: %ld\n", hpwd);
    computedHpwd2 = Xround(computedHpwd); 
    printf("ComputedLong Hpwd-Bravo: %ld\n",computedHpwd2);
    if (computedHpwd2 == hpwd) { printf("MATCH\n"); }    


    /* SIXTH, initialize history file; if a history file is present already, 
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
