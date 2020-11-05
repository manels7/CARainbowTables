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
#include "hashtable.h"
#define  KEY_LEN  16 /*  bytes*/

const EVP_CIPHER *h;
EVP_CIPHER_CTX *ctx;
const char *alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?!";//[a-zA-Z0-9?!]
const int alphabetLen = 64;
int l;
long rows;
unsigned __int128 universe;

int getInitalPwd(char startPwds[rows][l+1], char lastPwds[rows][l+1], char* pwd)
{
	for(int i = 0; i<rows; i++)
	{
		if(strcmp(startPwds[i], lastPwds[i]) == 0)
		{
			strcpy(pwd, startPwds[i]);
			return 1;
		}
	}
	return 0;
}
			
int compareHashes(char *originalHash, char *out)
{
	for(int j=0; j<16; j++)
		if(out[j] != originalHash[j])
			return 1;
	return 0;
}

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
	
	if (argc != 3)
	{
		printf("Missing args! Please run the application with the following arguments:\n");
		printf("%s <rainbow file> <hash>\n", programName);
		exit(0);
	}

	FILE *fp;
	h = EVP_aes_128_ecb();
	ctx = EVP_CIPHER_CTX_new();
	char *rainbowFileName = argv[1];
	char *hash = argv[2];
	char *len = malloc(sizeof(char));
	int k;
	char key[KEY_LEN];
	unsigned char out[KEY_LEN];
	int findings = 0, aesOp = 0;


	/**
	 * Read rainbow table
	 */
	if ((fp = fopen(rainbowFileName, "rb")) == NULL){
		printf("Error: opening file!");
		exit(1);
	}
	*len = (char)fgetc(fp);
	l = atoi(len);
   	fscanf(fp,"%d", &k);
   	fgetc(fp);//Removing \n that splits header from rainbow table
 
	rows = (int)pow(alphabetLen,l)/((k/2)-1);
	char pwd[l+1];
	universe = (unsigned __int128)pow(alphabetLen,l);

	int c, counter = 0, start = 1, index = 0;
	char pwdReaded[l+1];
	char startPwd[l+1];
	char startPwds[rows][l+1], lastPwds[rows][l+1]; //Remove these
	hashtable_t *ht = ht_create(rows);

	while ((c = getc(fp)) != EOF)
	{
		pwdReaded[counter] = (char)c;
		counter++;
		if (counter == l)
		{
			pwdReaded[counter] = '\0';
			counter = 0;
			if(start == 1)
			{
				strcpy(startPwds[index], pwdReaded);
				strcpy(startPwd, pwdReaded);
				start = 0;
			}
			else
			{
				strcpy(lastPwds[index], pwdReaded);
				ht_put(ht, pwdReaded, startPwds[index]);
				start = 1;
				index++;
			}
		}
	}
	fclose(fp);

	/**
	 * Read hash
	 */
	char *pos = hash;
	unsigned char originalHash[KEY_LEN];
	for (size_t count = 0; count < sizeof originalHash/sizeof *originalHash; count++) {
		sscanf(pos, "%2hhx", &originalHash[count]);
		pos += 2;
	}


	/**
	 * Print Readed Variables
	 */
	printf("Input file: %s\n", rainbowFileName);
	printf("Num rows: %ld\n", rows);
	printf("Password length: %d\n", l);
	printf("k value: %d\n", k);


	/**
	 * Password cracking
	 */
	for(int x = k-1; x>=0; x--)
	{
	 	unsigned char pwdH[strlen(originalHash)];
	 	strncpy(pwdH, originalHash, strlen(originalHash)); 
		for (int j = x; j<k; j++)
		{
			R(pwdH, pwd, j);

			if(j != k-1)//Does not run in the last cycle
			{
				H(pwdH, pwd);
				aesOp++;
			}
		}

		if(ht_get(ht, pwd))
		{
			findings++;
			char *inPwd = ht_get(ht, pwd);
			for (int i = 0; i<=x; i++)
			{
				
				H(out, inPwd);
				aesOp++;
				if(i!=x)
				{
					
					R(out, inPwd, i);
				}
			}

			if(compareHashes(originalHash, out) == 0)
			{
				printf("%s\n", inPwd);
				printf("Number of AES Operations: %d\n", aesOp);
				printf("False positives: %d\n", --findings);
				return 0;
			}
		}
	}
	printf("Not found!\n");
	printf("Number of AES Operations: %d\n", aesOp);
	printf("False positives: %d\n", findings);
}

