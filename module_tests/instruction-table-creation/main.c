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

//Program Constants:
char const historyFolderLocation[] = "./history"; //ensure this matches below lines
char const historyFilePath[] = "./history/historyfile"; //ensure path matches above
int const historyFileSize = 1200;


//Function Declarations:
void computeAlpha(Polynomial Poly, mpz_t* alpha, int numfeatures, char* password, mpz_t q, mpz_t r);
void computeBravo(Polynomial Poly, mpz_t* bravo, int numfeatures, char* password, mpz_t q, mpz_t r);


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

//Runs program initialization sequence
int initProgram(char* argv[], mpz_t q, mpz_t r) {
    int status = 0;
    /////////////////////////////
    //INITIALIZATION
    /////////////////////////////
    printf("Beginning Initialization.\n");

    //Make history directory if not already present on system:
    struct stat st ={0};
    /*if (stat(historyFolderLocation, &st) == -1) {
        mkdir(historyFolderLocation, 0700);
    }*/

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

  return status;
}



//MAIN-----------------------------------------------
int main(int argc, char* argv[])
{
    printf("Beginning 'Server' execution.\n");

    //Verify correct arguments
    if (argc != 2) {
        printf("ERROR: Incorrect arguments\nformat:\n");
        printf("./program inputfile.txt\n\nEXITING\n");
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
    
    return status;

}
