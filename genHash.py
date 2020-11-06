import sys
from utils import R, alphabet, keyGen
from Crypto.Cipher import AES

if len(sys.argv) != 2:
	print("Missing args! Please run the application with the following arguments:")
	print("python3 {} <password>".format(sys.argv[0]))
	exit()

key = bytes(keyGen(sys.argv[1]), 'UTF-8')

encryptor = AES.new(key, AES.MODE_ECB)
h = encryptor.encrypt(key)

print(h.hex())
