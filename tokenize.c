#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "server.h"


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
        // Change HERE:
	char delim[2] = { a_delim, '\0' }; // Change here
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


///ALTERNATE STRING SPLIT - Splits string to tokens based on delimiter
char **str_split_alt(const char* str, const char* delim, size_t* numtokens) {

    // copy the original string so that we don't overwrite parts of it
    // (don't do this if you don't need to keep the old line,
    // as this is less efficient)
    char *s = strdup(str);

    // these three variables are part of a very common idiom to
    // implement a dynamically-growing array

    size_t tokens_alloc = 1;
    size_t tokens_used = 0;
    char **tokens = calloc(tokens_alloc, sizeof(char*));
    char *token, *strtok_ctx;
    for (token = strtok_r(s, delim, &strtok_ctx);
            token != NULL;
            token = strtok_r(NULL, delim, &strtok_ctx)) {
        // check if we need to allocate more space for tokens
        if (tokens_used == tokens_alloc) {
            tokens_alloc *= 2;
            tokens = realloc(tokens, tokens_alloc * sizeof(char*));
        }
        tokens[tokens_used++] = strdup(token);
    }

    // cleanup
    if (tokens_used == 0) {
        free(tokens);
        tokens = NULL;
    } else {
        tokens = realloc(tokens, tokens_used * sizeof(char*));
    }
    *numtokens = tokens_used;
    free(s);
    return tokens;
}


///STRING SPLIT to Integer - Split comma separated string into integers
int str_to_ints(char* fstring, int features[]) {

    //char *line = "18,10,-2,20,10,4,2,8,6,17,23,20,27,7";
//    size_t linelen;

    char **tokens2;
    size_t numtokens;

    tokens2 = str_split_alt(fstring, ",", &numtokens);

    size_t i;
    for ( i = 0; i < numtokens; i++) {
//        printf("    token: \"%s\"\n", tokens2[i]);
        features[i] = atoi(tokens2[i]);
        free(tokens2[i]);
    }

    return numtokens;
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

    //END OF TESTING FOR HISTORY FILE
    //BEGIN TESTING FOR FEATURES STRING to INT

    char *featureString = "18,10,-2,20,10,4,2,8,6,17,23,20,27,7";
    int features[127];

    int numtokens = str_to_ints(featureString, features);
    printf("Number of Tokens: %d\n",numtokens);
    for ( i = 0; i < numtokens; i++) {
        printf("Integer %d: %d\n",i+1,features[i]);
    }


    return 0;
}
*/
