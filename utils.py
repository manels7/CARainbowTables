import random
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import padding
from cryptography.hazmat.backends import default_backend
from Crypto.Cipher import AES

alphabet = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?!'#[a-zA-Z0-9?!]
padder = padding.PKCS7(algorithms.AES.block_size).padder()

def R(ite, hashX, passLen, alphabet): 
	newHashX = (int.from_bytes(hashX, 'big') + ite)#.to_bytes(len(hashX), 'big')
	msg = ""
	for x in range(0,passLen):
		shift = int(x*((len(hashX)*8)/passLen))
		value = newHashX >> shift
		value = value % len(alphabet)
		msg += alphabet[value]
	return msg

def keyGen(pwd):
	res = pwd
	while len(res) < 16:
		res += pwd
	return res[:16]

def seedGen(alphabet, passLen):
	seed = []
	for x in range(0,passLen):
		seed.append(alphabet[random.randint(0,len(alphabet)-1)])
	return ''.join(seed)
'''
def genLine(S1, Sk, passLen, alphabetLen):
	#convert S1 and Sk into a compressed representation to store in the file
	return bytes()

def readLine(line):
	#convert line into S1 and Sk
	return S1, Sk
'''
def encriptV1(key, l, ite):
	cipher = Cipher(algorithms.AES(key), modes.ECB(), default_backend())
	encryptor = cipher.encryptor()
	h = encryptor.update(padder.update(key))
	return R(ite, h, l, alphabet)

def encriptV2(key, l, ite):
	encryptor = AES.new(key, AES.MODE_ECB)
	h = encryptor.encrypt(key)
	return R(ite, h, l, alphabet)