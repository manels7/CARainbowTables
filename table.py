import sys
from utils import R, seedGen, keyGen, alphabet, encriptV1, encriptV2

if len(sys.argv) != 4:
	print("Missing args! Please run the application with the following arguments:")
	print("python3 {} <password length> <rainbow table size> <output file name>".format(sys.argv[0]))
	exit()

#Args
rainbowFileName = sys.argv[3]
l = int(sys.argv[1])
s = int(sys.argv[2])

#Validations (some)
if l < 1 or l > 16:
	print("Password length not valid!")
	exit()
if s <1:
	print("Rainbow table size not valid!")
	exit()

#Variables
alphabetLen = len(alphabet)
numRows = int((16*(2**s))/(l*2)) 
k = (int((alphabetLen**l)/numRows)+1)*2

print("Output file: {}".format(rainbowFileName))
print("Num rows: {}".format(numRows))
print("Password length: {}".format(l))
print("s value: {}".format(s))
print("k value: {}".format(k))

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
		#pwd = encriptV1(key, l, ite)#Code provided in classe but not efficient
		pwd, h = encriptV2(key, l, ite)
	lastPwds.append(pwd)
	print("Interaction {} of {}".format(row, numRows))

#This for is not time consuming
file = open(rainbowFileName, 'wb')
file.write(bytes(str(l), 'UTF-8'))
file.write(bytes(str(k), 'UTF-8'))
file.write(bytes("\n", 'UTF-8'))
for seed, pwd in zip(usedSeeds, lastPwds):
	file.write(bytes(seed, 'UTF-8'))
	file.write(bytes(pwd, 'UTF-8'))
file.close()