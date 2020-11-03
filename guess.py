import sys
from utils import R, seedGen, keyGen, alphabet, encriptV2
from Crypto.Cipher import AES

if len(sys.argv) != 3:
	print("Missing args! Please run the application with the following arguments:")
	print("python3 {} <rainbow file> <hash>".format(sys.argv[0]))
	exit()

#Resultado para o stdout
rainbowTable = {}
rainbowFileName = sys.argv[1]
h = bytearray.fromhex(sys.argv[2])
l = None
k = b''

with open(rainbowFileName, 'rb') as file:
	l = int(file.read(1))
	while True:
		byte = file.read(1)
		if byte == b"\n":
			break
		k += byte
	k = int(k)

	initialPwd = b''
	lastPwd = b''
	while True:
		byte = file.read(1)
		if len(initialPwd) < l:
			initialPwd += byte
		elif len(lastPwd) < l:
			lastPwd += byte

		if len(lastPwd) == l:
			rainbowTable[str(lastPwd.decode("utf-8"))] = str(initialPwd.decode("utf-8"))
			initialPwd = b''
			lastPwd = b''

		if byte == b"":
			break

print("Intput file: {}".format(rainbowFileName))
print("Num rows: {}".format(len(rainbowTable)))
print("Password length: {}".format(l))
print("k value: {}".format(k))
STOP = False
password = ""

for x in range(k-1, -1, -1):
	hh = h
	for ite in range(x, k):
		pwd = R(ite, hh, l, alphabet)
		key = bytes(keyGen(pwd), 'UTF-8')
		encryptor = AES.new(key, AES.MODE_ECB)
		hh = encryptor.encrypt(key)

	if pwd in rainbowTable:
		pwd = rainbowTable[pwd]
		for ite in range(0, x):
			key = bytes(keyGen(pwd), 'UTF-8')
			encryptor = AES.new(key, AES.MODE_ECB)
			z = encryptor.encrypt(key)
			pwd = R(ite, z, l, alphabet)

		key = bytes(keyGen(pwd), 'UTF-8')
		encryptor = AES.new(key, AES.MODE_ECB)
		z = encryptor.encrypt(key)
		#print(z.hex())
		#print(h.hex())
		#print(pwd)
		if h.hex() == z.hex():
			password = pwd
			STOP = True
			break
	if STOP:
		break

print(password)