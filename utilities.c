/* Copies string up to length 'length' from source to target; position within 
   string source can change depending upon with round, 'rnd'; position within 
   string target is always at the beginning of the string */
int copy_in(unsigned char *source, unsigned char *target, int rnd, int length) {
        int c = (rnd - 1) * length;
        int d = 0;
        int end = c + length;
        while (source[c] != '\0' && c < end ) {
                target[d] = source[c];
                c++;
                d++;
        }
        target[c] = '\0';
        return d;
}

//Copies string up to length outlength from source to target
void copy_out(unsigned char *source, unsigned char *target, int rnd, int length, int outlength) {
        int c = (rnd - 1) * length;
        int d = 0;
        int end = outlength;
        while ( source[d] != '\0' && d < end ) {
                target[c] = source[d];
                c++;
                d++;
        }
        target[c] = '\0';
}

/* Copies password from source to target, ignoring the newline character, thus 
   removing newline from the copied password returned in target */
int copy_pwd(char *target, char *source) {
        int c = 0;
        while ( source[c] != '\n' && source[c] != '\0' ) {
                target[c] = source[c];
                c++;
        }
        target[c] = '\0';
        return c;
}

