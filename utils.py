def R(ite, hashX, passLen, alphabetLen): 
	newHashX = (int.from_bytes(hashX, 'big') + ite)#.to_bytes(len(hashX), 'big')
	msg = []
	for x in range(0,passLen):
		shift = int(x*((len(hashX)*8)/passLen))
		value = newHashX >> shift
		value = value % alphabetLen
		msg.append(value)
	return msg

def genLine(S1, Sk, passLen, alphabetLen):
	#convert S1 and Sk into a compressed representation to store in the file
	return bytes()

def readLine(line):
	#convert line into S1 and Sk
	return S1, Sk