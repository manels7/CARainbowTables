import random

def R(ite, hashX, passLen, alphabet): 
	newHashX = (int.from_bytes(hashX, 'big') + ite)#.to_bytes(len(hashX), 'big')
	msg = ""
	for x in range(0,passLen):
		shift = int(x*((len(hashX)*8)/passLen))
		value = newHashX >> shift
		value = value % len(alphabet)
		msg+= alphabet[value]
	return msg

def keyGen(pwd):
	res = pwd
	while len(res) < 16:
		res += pwd
	return res[:16]

def seedGen(alphabet, passLen):
	seed = ""
	for x in range(0,passLen):
		seed += alphabet[random.randint(0,len(alphabet)-1)]
	return seed

def genLine(S1, Sk, passLen, alphabetLen):
	#convert S1 and Sk into a compressed representation to store in the file
	return bytes()

def readLine(line):
	#convert line into S1 and Sk
	return S1, Sk