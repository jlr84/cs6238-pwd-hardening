#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

void copy_string(char source[], char target[]);
void copy_ustring(unsigned char source[], unsigned char target[], int round);
void copy_ustringptr(unsigned char * source, unsigned char * target, int round);
int copy_in(unsigned char *source, unsigned char *target, int rnd, int length);
void copy_out(unsigned char *source, unsigned char *target, int rnd, int length, int outlength); 
int encrypt_buf_to_file(unsigned char *mybufin, FILE *out);
int decrypt_file_to_buffer(FILE *in, unsigned char *mybufout);

int main (void)
{
  /* Message to be encrypted */
  unsigned char mybufin[] = "The quick brown fox jumps over the lazy dog\n1aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff2aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff3aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff4aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff5aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff6aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff7aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff8aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff9aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff10aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff11aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff12aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff13aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff14aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff15aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff16aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff17aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffa18aaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff19aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff20aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff21aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff22aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff23aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff24aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff25aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff26aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff27aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff28aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff29aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff30aaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff";
  unsigned char mybufout[5000];
  unsigned char mybufdecrypt[5000];
  unsigned char *mybufinptr = (unsigned char *)&mybufin;
  unsigned char *mybufoutptr = (unsigned char *)&mybufout;
  unsigned char *mybufdecryptptr = (unsigned char *)&mybufdecrypt;
/*
  char mytarget[5000];
  char mysource[] = "The quick brown fox jumps over the lazy dog\naaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff";

  unsigned char myutarget[5000];
  unsigned char myusource[] = "The quick brown fox jumps over the lazy dog\naaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffffaaaaaaaaaabbbbbbbbbbcccccccccceeeeeeeeeeffffffffff";
  unsigned char *myutargetptr = (unsigned char *)&myutarget;
  unsigned char *myusourceptr = (unsigned char *)&myusource;
  long slength = strlen( myusource );
  printf("SLength: %lu\n", slength);
  long slengthptr = strlen( myusourceptr );
  printf("SLengthptr: %lu\n", slengthptr);
  int rounds = ( slengthptr + (100 - 1)) / 100; // Dividing with roundup
  printf("Rounds3: %d\n", rounds); 
*/ 
/*
  copy_string(mysource, mytarget);
  printf("Source: %s\nTarget: %s\n", mysource, mytarget);
  
  int round = 1;
  for ( round; round < 4; round = round + 1 ) {
	copy_ustring(myusource, myutarget, round);
	printf("\n\nRound: %d\nUSource: %s\nUTarget: %s\n", round,myusource, myutarget);
  }
*/
/*  int round2 = 1;
  for ( round2; round2 <= rounds; round2 = round2 + 1 ) {
        copy_ustringptr(myusourceptr, myutargetptr, round2);
        printf("\n\nRound: %d\nUSourcePtr: %s\nUTargetPtr: %s\n", round2, myusourceptr, myutargetptr);
  }
*/
 
  FILE* infile; 
  infile = fopen("./plaintext", "r");
  FILE* outfile;
  char const outfilename[] ="./outfile4";
  outfile = fopen(outfilename, "w+");

  printf("\n\nMybufin: %s\nMybufout: %s\nMybufdecrypt: %s\n",mybufinptr, mybufoutptr, mybufdecryptptr);

  // 1 for encrypt 0 for decrypt, -1 for leave unchanged
  int returncode;
  printf("ENCRYPTING BUFFER TO FILE %s\n", outfilename);
  returncode = encrypt_buf_to_file(mybufinptr, outfile);
//  returncode = do_crypt( infile, outfile, 1, mybufinptr, mybufoutptr ); 
  fclose(infile);
  fclose(outfile);

  printf("\n\nENCRYPTION COMPLETED .... STARTING DECRYPTION\nOpening file: %s\n", outfilename);

  infile = fopen(outfilename, "r");
  outfile = fopen("./decrypted", "w+");
  decrypt_file_to_buffer(infile, mybufdecryptptr);
//  returncode = do_crypt( infile, outfile, 0, mybufoutptr, mybufdecryptptr );
  fclose(infile);
  fclose(outfile); 

  printf("\n\nFINAL___\nMybufin(Original): %s\nMyDecrypted: %s\n",mybufinptr, mybufdecryptptr);

return 0;

}


int do_crypt(FILE *in, FILE *out, int do_encrypt, unsigned char *mybufin, unsigned char *mybufout)
        {
        /* Allow enough space in output buffer for additional block */
        unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
	unsigned char *inbufptr = (unsigned char *)&inbuf;
	unsigned char *outbufptr = (unsigned char *)&outbuf;
        int inlen, outlen;
	int outtotal = 0;
        EVP_CIPHER_CTX ctx;
        /* Bogus key and IV: we'd normally set these from
         * another source.
         */
        unsigned char key[] = "0123456789abcdeF";
        unsigned char iv[] = "1234567887654321";

        /* Don't set key or IV right away; we want to check lengths */
        EVP_CIPHER_CTX_init(&ctx);
        EVP_CipherInit_ex(&ctx, EVP_aes_128_cbc(), NULL, NULL, NULL,
                do_encrypt);
        OPENSSL_assert(EVP_CIPHER_CTX_key_length(&ctx) == 16);
        OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == 16);

        /* Now we can set key and IV */
        EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, do_encrypt);

//        for(;;)
//                {
		long inbuflength = strlen( mybufin );
		int rnds = ( inbuflength + (1024 - 1)) / 1024;
		printf("InBufLength: %lu\nRounds: %d\n", inbuflength, rnds);
		int i;
		for ( i = 1; i <= rnds; i = i + 1) {
			inlen = copy_in(mybufin, inbufptr, i, 1024);
			printf("inlen: %d\n", inlen);

			if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
                        {
                        /* Error */
                        EVP_CIPHER_CTX_cleanup(&ctx);
                        return 0;
                        }
//                fwrite(outbuf, 1, outlen, out);
  			outtotal = outtotal + outlen;
			copy_out(outbufptr, mybufout, i, 1024, outlen);
                	}
//		}
        if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
                {
                // Error //
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
                }
//        fwrite(outbuf, 1, outlen, out);
//	long outbuflength = strlen( mybufout );
	copy_out(outbufptr, mybufout, 2, outtotal, outlen);
	outtotal = outtotal + outlen;
        EVP_CIPHER_CTX_cleanup(&ctx);

        printf("\n\nMybufin: %s\nMybufout: %s\nOutTotal: %d\n",mybufin, mybufout, outtotal);
        return 1;
        }


/*	for(;;)
		{
                inlen = fread(inbuf, 1, 1024, in);
		printf("inlen: %d\n", inlen);		
                if(inlen <= 0) break;
                if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
                        {
                        // Error //
                        EVP_CIPHER_CTX_cleanup(&ctx);
                        return 0;
                        }
            	fwrite(outbuf, 1, outlen, out);
		
                }
        if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
                {
                // Error //
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
                }
        fwrite(outbuf, 1, outlen, out);

        EVP_CIPHER_CTX_cleanup(&ctx);

	printf("\n\nMybufin: %s\nMybufout: %s\n",mybufin, mybufout);
        return 1;
        }
*/

//DECRYPT FROM FILE TO BUFFER
int decrypt_file_to_buffer(FILE *in, unsigned char *mybufout) {
	
	int do_encrypt = 0; //1 for encrypt; 0 for decrypt

        /* Allow enough space in output buffer for additional block */
        unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
        int inlen, outlen;
        EVP_CIPHER_CTX ctx;
        /* Bogus key and IV: we'd normally set these from
         * another source.
         */
        unsigned char key[] = "0123456789abcdeF";
        unsigned char iv[] = "1234567887654321";

        /* Don't set key or IV right away; we want to check lengths */
        EVP_CIPHER_CTX_init(&ctx);
        EVP_CipherInit_ex(&ctx, EVP_aes_128_cbc(), NULL, NULL, NULL,
                do_encrypt);
        OPENSSL_assert(EVP_CIPHER_CTX_key_length(&ctx) == 16);
        OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == 16);

        /* Now we can set key and IV */
        EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, do_encrypt);

	int i;
        for(i = 1; i > 0; i = i + 1) {
                inlen = fread(inbuf, 1, 1024, in);
                printf("\nInlen(characters read): %d\n", inlen);
                if(inlen <= 0) break;
                if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen)) {
                        /* Error */
                        EVP_CIPHER_CTX_cleanup(&ctx);
                        return 0;
                }
		printf("\nCOPY: i=%d; oulen=%d\n",i,outlen);
		printf("BEFORE COPY___\nOutbuf: %s\nMybufout: %s",outbuf, mybufout);
		copy_out(outbuf, mybufout, i, 1024, inlen); 
		
		printf("AFTER  COPY___\nOutbuf: %s\nMybufout: %s",outbuf, mybufout);
//                fwrite(outbuf, 1, outlen, out);
        }
        if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen)) {
                /* Error */
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
        }
//	int finallength = strlen( mybufout );
//	copy_out(outbuf, mybufout, 2, finallength, outlen);
//        fwrite(outbuf, 1, outlen, out);

        EVP_CIPHER_CTX_cleanup(&ctx);

    //    printf("\n\nInbuf: %s\nMybufout: %s\n",inbuf, mybufout);
        return 1;
}

//ENCRYPT FROM BUFFER TO FILE
int encrypt_buf_to_file(unsigned char *mybufin, FILE *out) {
	
	int do_encrypt = 1; // 1 for encrypt; 0 for decrypt
 
        /* Allow enough space in output buffer for additional block */
        unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
        unsigned char *inbufptr = (unsigned char *)&inbuf;
        //unsigned char *outbufptr = (unsigned char *)&outbuf;
        int inlen, outlen;
        int outtotal = 0;
        EVP_CIPHER_CTX ctx;
        /* Bogus key and IV: we'd normally set these from
         * another source.
         */
        unsigned char key[] = "0123456789abcdeF";
        unsigned char iv[] = "1234567887654321";

        /* Don't set key or IV right away; we want to check lengths */
        EVP_CIPHER_CTX_init(&ctx);
        EVP_CipherInit_ex(&ctx, EVP_aes_128_cbc(), NULL, NULL, NULL,
                do_encrypt);
        OPENSSL_assert(EVP_CIPHER_CTX_key_length(&ctx) == 16);
        OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == 16);

        /* Now we can set key and IV */
        EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, do_encrypt);

//        for(;;)
//                {
                long inbuflength = strlen( mybufin );
                int rnds = ( inbuflength + (1024 - 1)) / 1024;
                printf("InBufLength: %lu\nRounds: %d\n", inbuflength, rnds);
                int i;
                for ( i = 1; i <= rnds; i = i + 1) {
                        inlen = copy_in(mybufin, inbufptr, i, 1024);
                      printf("inlen(characters copied): %d\n", inlen);

                        if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen)) {
				/* Error */
				EVP_CIPHER_CTX_cleanup(&ctx);
				return 0;
                        }
			fwrite(outbuf, 1, outlen, out);
                        outtotal = outtotal + outlen;
                        //copy_out(outbufptr, mybufout, i, 1024, outlen);
		}
//              }
        if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen)) {
                // Error //
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
        }
//        fwrite(outbuf, 1, outlen, out);
//      long outbuflength = strlen( mybufout );
//        copy_out(outbufptr, mybufout, 2, outtotal, outlen);
//        outtotal = outtotal + outlen;
        EVP_CIPHER_CTX_cleanup(&ctx);

//        printf("\n\nMybufin: %s\n\nEND OF ENCRYPTION\n", mybufin);
        return 1;
        }


// Copies string from source to target
void copy_string(char source[], char target[]) {
	int c = 0;
	while (source[c] != '\0' && c < 100) {
		target[c] = source[c];
		c++;
	}
	target[c] = '\0';
}
void copy_ustring(unsigned char source[], unsigned char target[], int rnd) {
	int c = (rnd - 1) * 100; 
	int d = 0;
        int end = c + 100;
	while (source[c] != '\0' && c < end ) {
		target[d] = source[c];
		c++;
		d++;
	}
	target[d] = '\0';
}
void copy_ustringptr(unsigned char *source, unsigned char *target, int rnd) {
        int c = (rnd - 1) * 100;
        int d = 0;
        int end = c + 100;
        while (source[c] != '\0' && c < end ) {
                target[d] = source[c];
                c++;
                d++;
        }
        target[d] = '\0';
}

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

void copy_out(unsigned char *source, unsigned char *target, int rnd, int length, int outlength) {
//	printf("\nSTART___\nSource: %s\nTarget: %s\nRound: %d\nLength: %d\nOutLength: %d\n",source, target, rnd, length, outlength);

        int c = (rnd - 1) * length;
	int d = (rnd - 1) * 16;
  //      printf("\nRnd: %d | C: %d\n",rnd,c);
//	int end = c + outlength;
	int end2 = outlength + 27;
//	printf("END: %d\n",end);
        while ( source[d] != '\0' && d < end2 ) {
                target[c] = source[d];
                c++;
                d++;
        }
    	//printf("\nC=%d\n",c);
        target[c] = '\0';
//	printf("\nEND___\nSource: %s\nTarget: %s\nRound: %d\nLength: %d\nOutLength: %d\n",source, target, rnd, length, outlength);
}

int do_crypt_file(FILE *in, FILE *out, int do_encrypt, unsigned char *mybufin, unsigned char *mybufout)
        {
        /* Allow enough space in output buffer for additional block */
        unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
        int inlen, outlen;
        EVP_CIPHER_CTX ctx;
        /* Bogus key and IV: we'd normally set these from
         * another source.
         */
        unsigned char key[] = "0123456789abcdeF";
        unsigned char iv[] = "1234567887654321";

        /* Don't set key or IV right away; we want to check lengths */
        EVP_CIPHER_CTX_init(&ctx);
        EVP_CipherInit_ex(&ctx, EVP_aes_128_cbc(), NULL, NULL, NULL,
                do_encrypt);
        OPENSSL_assert(EVP_CIPHER_CTX_key_length(&ctx) == 16);
        OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == 16);

        /* Now we can set key and IV */
        EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, do_encrypt);

        for(;;)
                {
                inlen = fread(inbuf, 1, 1024, in);
                printf("inlen: %d\n", inlen);
                if(inlen <= 0) break;
                if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
                        {
                        /* Error */
                        EVP_CIPHER_CTX_cleanup(&ctx);
                        return 0;
                        }
                fwrite(outbuf, 1, outlen, out);

                }
        if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
                {
                /* Error */
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
                }
        fwrite(outbuf, 1, outlen, out);

        EVP_CIPHER_CTX_cleanup(&ctx);

        printf("\n\nMybufin: %s\nMybufout: %s\n",mybufin, mybufout);
        return 1;
}
