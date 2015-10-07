/* This file encrypts or decrypts from file to file 
   Not used in project. */ 

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

int main (void)
{
  /* Message to be encrypted */
  unsigned char *plaintext =
                (unsigned char *)"The quick brown fox jumps over the lazy dog";

  unsigned char ciphertext[128];
  FILE* infile; 
  infile = fopen("./plaintext", "r");
  FILE* outfile;
  outfile = fopen("./outfile", "w+");

  // 1 for encrypt 0 for decrypt, -1 for leave unchanged
  int returncode;
  returncode = do_crypt( infile, outfile, 1 ); 
  return 0;
}


 int do_crypt(FILE *in, FILE *out, int do_encrypt)
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
        return 1;
        }
