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
unsigned __int128 universe;

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
}

void H(char *out, char *pwd)
{
	//Key generation
	char key[KEY_LEN];
	keyGen(key, pwd);

	//Cipher with AES
	int len;
	EVP_EncryptInit(ctx, h, key, 0);
	EVP_EncryptUpdate(ctx, out, &len, key, 16);
	EVP_CIPHER_CTX_cleanup(ctx);
}

void R(char *out, char *pwd, int k)
{
	//Transform binary into a integer (long long)
	unsigned __int128 hashValue = *((unsigned __int128*)out); 
	hashValue += k;
	int newPwdValue = hashValue % universe;
	getPwd(pwd, newPwdValue);
}

int main(int argc, char *argv[])
{
	/**
	 * Variables
	 */
	char *programName = argv[0];
	
	if (argc != 4 && argc != 5)
	{
		printf("Missing args! Please run the application with the following arguments:\n");
		printf("%s <password length> <rainbow table size> <output file name> ", programName);
		printf("<split processing (optional)>\n");
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
	universe = (unsigned __int128)pow(alphabetLen,l);

	printf("Output file: %s\n", rainbowFileName);
	printf("Num rows: %ld\n", rows);
	printf("Password length: %d\n", l);
	printf("s value: %d\n", s);
	printf("k value: %d\n", k);

	fp = fopen(rainbowFileName, "wb+");
	fprintf(fp, "%d%d\n", l, k);


	/**
	 * Split processing
	 * This is used to create parts of the rainbow table and then
	 * concat all the outputs files (Carefull)
	 */
	if (argc == 5)
	{
		int splitR = atoi(argv[4]);
		rows = rows/splitR;
		printf("Split processing: %d\n", splitR);
		printf("New num rows: %ld\n", rows);
	}


	/**
	 * Rainbow table generation
	 */
	char pwd[l+1];
	char out[KEY_LEN];
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
			H(out, pwd);
			R(out, pwd, j);
		}
		fprintf(fp, "%s", pwd);
	}
	fclose(fp);
}	

