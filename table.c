#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <tgmath.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <collectc/hashtable.h>
#include "hashtable.h"
#define  KEY_LEN  16 /*  bytes*/

const EVP_CIPHER *h;
EVP_CIPHER_CTX *ctx;
const char *alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?!";//[a-zA-Z0-9?!]
const int alphabetLen = 64;
int l;
long rows;

void getPwd(char* pwd, long pwdIndex)
{
	long value = pwdIndex;
	for(int x = l-1; x >= 0; x--)
	{
		int charIndex = value % alphabetLen;
		value = (long)value / alphabetLen;
		pwd[x] = alphabet[charIndex];
	}
	pwd[l] = '\0';
}

void keyGen(char* key, char* pwd)
{
	int pwdCounter = 0;
 	for (int i = 0; i < 16; i++)
 	{
    	if (pwdCounter == l)
 			pwdCounter = 0;
        key[i] = pwd[pwdCounter];
    	pwdCounter++;
    }
    key[16] = '\0';
}


int main(int argc, char *argv[])
{
	/**
	 * Variables
	 */
	char *programName = argv[0];
	
	if (argc != 4)
	{
		printf("Missing args! Please run the application with the following arguments:\n");
		printf("%s <password length> <rainbow table size> <output file name>\n", programName);
		exit(0);
	}
	HashTable *table;
	FILE *fp;
	h = EVP_aes_128_ecb();
	ctx = EVP_CIPHER_CTX_new();
	char *rainbowFileName = argv[3];
	int s = atoi(argv[2]);
	l = atoi(argv[1]);
	rows = (16 * pow(2, s)) / (l*2);
	int k = ((pow(alphabetLen, l)/rows) + 1) * 2;

	printf("Output file: %s\n", rainbowFileName);
	printf("Num rows: %ld\n", rows);
	printf("Password length: %d\n", l);
	printf("s value: %d\n", s);
	printf("k value: %d\n", k);

	fp = fopen(rainbowFileName, "wb+");
	fprintf(fp, "%d%d\n", l, k);

	/**
	 * Rainbow table generation
	 */
	char pwd[l];
	char key[16];
	char out[16];
	unsigned __int128 universe = (unsigned __int128)pow(alphabetLen,l);
	srand(time(NULL));
	hashtable_t *ht = ht_create(rows);

	for(int i = 0; i < rows; i++)
	{
		//Generate Seed		
		while (1)
		{	
			int seed = rand() % (long)universe;
			getPwd(pwd, seed);
			if(!ht_get(ht, pwd))
				break;
		}
		fprintf(fp, "%s", pwd);
		ht_put(ht, pwd, pwd);

		//Compute
		for(int j = 0; j < k; j++)
		{
			//Key generation
			keyGen(key, pwd);

			//Cipher with AES
			int len;
			EVP_EncryptInit(ctx, h, key, 0);
			EVP_EncryptUpdate(ctx, out, &len, key, 16);
			EVP_CIPHER_CTX_cleanup(ctx);
		
			//Transform binary into a integer (long long)
			unsigned __int128 *hashValue = (unsigned __int128*)&out; 
			*hashValue += j;
			int newPwdValue = *hashValue % universe;
			getPwd(pwd, newPwdValue);
		}
		fprintf(fp, "%s", pwd);
	}
	fclose(fp);
}	

