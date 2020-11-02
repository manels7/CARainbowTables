import sys
from utils import R, seedGen, keyGen
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import padding
from cryptography.hazmat.backends import default_backend

if len(sys.argv) != 4:
	print("Missing args! Please run the application with the following arguments:")
	print("python3 {} <password length> <rainbow table size> <output file name>".format(sys.argv[0]))
	exit()

#Validations
print("to do")

#Variables
alphabet = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?!'#[a-zA-Z0-9?!]
alphabetLen = len(alphabet)
rainbowFileName = sys.argv[3]
l = int(sys.argv[1])
s = int(sys.argv[2])
numRows = int((16*(2**s))/(l*2)) 
k = int((alphabetLen**l)/numRows)+1

print("Output file: {}".format(rainbowFileName))
print("Num rows: {}".format(numRows))
print("Password length: {}".format(l))
print("s value: {}".format(s))
print("k value: {}".format(k))

padder = padding.PKCS7(algorithms.AES.block_size).padder()
#Generate RAINBOW TABLE
usedSeeds = []
lastPwds = []
for row in range(0,numRows):
	while True:
		seed = seedGen(alphabet, l)
		if seed not in usedSeeds:
			usedSeeds.append(seed)
			break

	pwd = seed
	for ite in range(0, k):
		key = bytes(keyGen(pwd), 'UTF-8')
		cipher = Cipher(algorithms.AES(key), modes.ECB(), default_backend())
		encryptor = cipher.encryptor()
		h = encryptor.update(padder.update(key))
		pwd = R(ite, h, l, alphabet)
	lastPwds.append(pwd)

#This for is not time consuming
file = open(rainbowFileName, 'wb')
for seed, pwd in zip(usedSeeds, lastPwds):
	file.write(bytes(seed, 'UTF-8'))
	file.write(bytes(pwd, 'UTF-8'))
file.close()