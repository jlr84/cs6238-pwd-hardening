#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Type Definition; this is the same as in 'server.c'; only placed 
   here for testing; uncomment this and 'main', below,  to test 
   this "standalone" */
/*typedef struct{
    char *validation;
    char *size;
    char *line1;
    char *line2;
    char *line3;
    char *line4;
    char *line5;
    char *end;
}history;
*/

//Function declarations;
char** str_split(char* a_str, const char a_delim);

//STRING SPLIT - Split string based on delimiter
char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}


//MAIN provided for testing; uncomment for testing this 'standalone'
/*int main()
{
    //Prepare my string for testing; NOTE: Same as in 'createFreshHistory()'
    char mybufin[1000] = "VALID\n0\nNA\nNA\nNA\nNA\nNA\nEND_OF_FILE\n";
    char filler[] ="aaaa";

    //Pad to length=1000
    int i;
    int length = strlen( mybufin );
    for ( i = length; i < 1000; i = i + 1) {
        mybufin[i] = filler[1];
    }

    //Create pointer to input buffer just created
    char *mybufinptr = (char *)&mybufin;

    printf("Starting Buffer:\n%s\n\n", mybufinptr);

    //Declare a new history
    history currentHistory;

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

   //Check tokenized input to verify successful tokenization [and decryption]
    if ( strcmp(currentHistory.validation,"VALID") == 0 && strcmp(currentHistory.end,"END_OF_FILE") == 0 ) {
	printf("\nVALIDATED SUCCESSFULLY\n");
    } else {
	printf("\nERROR3\n");
    }

    return 0;
}
*/
