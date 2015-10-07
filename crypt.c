#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>


//Function Declarations:
int decrypt_file_to_buf(FILE *in, unsigned char *mybufout);
int encrypt_buf_to_file(unsigned char *mybufin, FILE *out);
int copy_in(unsigned char *source, unsigned char *target, int rnd, int length);
void copy_out(unsigned char *source, unsigned char *target, int rnd, int length, int outlength);

//DECRYPT FROM FILE TO BUFFER
int decrypt_file_to_buf(FILE *in, unsigned char *mybufout) {
	
	int do_encrypt = 0; //1 for encrypt; 0 for decrypt

        /* Allow enough space in output buffer for additional block */
        unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
        int inlen, outlen;
        EVP_CIPHER_CTX ctx;
        
	/* Bogus key and IV: we'd normally set these from
           another source. */
        unsigned char key[] = "0123456789abcdeF";
        unsigned char iv[] = "1234567887654321";

        /* Don't set key or IV right away; we want to check lengths */
        EVP_CIPHER_CTX_init(&ctx);
        EVP_CipherInit_ex(&ctx, EVP_aes_128_cbc(), NULL, NULL, NULL, do_encrypt);
        OPENSSL_assert(EVP_CIPHER_CTX_key_length(&ctx) == 16);
        OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == 16);

        /* Now we can set key and IV */
        EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, do_encrypt);

	int i;
	int totalread = 0;
        for(i = 1; i > 0; i = i + 1) {
                inlen = fread(inbuf, 1, 1024, in);
		totalread = totalread + inlen;
                if(inlen <= 0) break;
                if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen)) {
                        /* Error */
                        EVP_CIPHER_CTX_cleanup(&ctx);
                        return 0;
                }
		copy_out(outbuf, mybufout, i, 1024, inlen); 
        }
        if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen)) {
                /* Error */
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
        }

        EVP_CIPHER_CTX_cleanup(&ctx);

	printf("\nTotal Characters Read: %d\n",totalread);
        return totalread;
}

//ENCRYPT FROM BUFFER TO FILE
int encrypt_buf_to_file(unsigned char *mybufin, FILE *out) {
	
	int do_encrypt = 1; // 1 for encrypt; 0 for decrypt
 
        /* Allow enough space in output buffer for additional block */
        unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
        unsigned char *inbufptr = (unsigned char *)&inbuf;
        int inlen, outlen;
        EVP_CIPHER_CTX ctx;
        
	/* Bogus key and IV: we'd normally set these from
           another source. */
        unsigned char key[] = "0123456789abcdeF";
        unsigned char iv[] = "1234567887654321";

        /* Don't set key or IV right away; we want to check lengths */
        EVP_CIPHER_CTX_init(&ctx);
        EVP_CipherInit_ex(&ctx, EVP_aes_128_cbc(), NULL, NULL, NULL, do_encrypt);
        OPENSSL_assert(EVP_CIPHER_CTX_key_length(&ctx) == 16);
        OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == 16);

        /* Now we can set key and IV */
        EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, do_encrypt);

        long inbuflength = strlen( mybufin );
        int rnds = ( inbuflength + (1024 - 1)) / 1024;
        int i;
        for ( i = 1; i <= rnds; i = i + 1) {
        	inlen = copy_in(mybufin, inbufptr, i, 1024);
                if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen)) {
			// Error //
			EVP_CIPHER_CTX_cleanup(&ctx);
			return 0;
		}
		fwrite(outbuf, 1, outlen, out);
	}
        if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen)) {
                // Error //
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
        }
        EVP_CIPHER_CTX_cleanup(&ctx);

        return 1;
}

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

// MAIN for testing -- un-comment to run crypt.c by itself
/*int main (void)
{
  // Message to be encrypted // 
  // !!Currently having an error with input longer than 1077 bytes.. decrypts fine, but missing     some of the output... troubleshoot later. This message will work; anything longer will get     messed up. //
  unsigned char mybufin[] = "The quick brown fox jumps over the lazy dog\n1aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff2aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff3aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff4aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff5aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff6aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff7aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff8aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff9aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff10aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff11aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff12aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff13aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff14aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff15aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff16aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff17aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffa18aaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff19aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff2";
  unsigned char mybufout[5000];
  unsigned char mybufdecrypt[5000];
  unsigned char *mybufinptr = (unsigned char *)&mybufin;
  unsigned char *mybufoutptr = (unsigned char *)&mybufout;
  unsigned char *mybufdecryptptr = (unsigned char *)&mybufdecrypt;
  
  FILE* f;
  char const outfilename[] ="./outfile4";
  f = fopen(outfilename, "w+");

  printf("\nBEGINNING ENCRYPTION; Starting with...\nInternal Input Buffer: %s\nOutput Buffer: %s\nDecryption Buffer: %s\n",mybufinptr, mybufoutptr, mybufdecryptptr);

  // 1 for encrypt 0 for decrypt, -1 for leave unchanged
  int returncode;
  printf("ENCRYPTING BUFFER TO FILE %s\n", outfilename);
  returncode = encrypt_buf_to_file(mybufinptr, f);
  fclose(f);

  printf("\n\nENCRYPTION COMPLETED .... STARTING DECRYPTION\nOpening file: %s\n", outfilename);

  f = fopen(outfilename, "r");
  decrypt_file_to_buf(f, mybufdecryptptr);
  fclose(f);

  printf("\n\nCOMPLETED:\nInput Buffer (Original): %s\nDecrypted Buffer: %s\n",mybufinptr, mybufdecryptptr);

return 0;

}
*/
