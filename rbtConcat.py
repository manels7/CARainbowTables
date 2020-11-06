import glob
import sys

if len(sys.argv) != 3:
	print("Missing args! Please run the application with the following arguments:")
	print("python3 {} <rainbow table location> <pattern>".format(sys.argv[0]))
	exit()

#Args
rbtLocation = sys.argv[1]
rbtPattern = sys.argv[2]

rbtFiles = []
for file in glob.glob('{}/{}*'.format(rbtLocation,rbtPattern)):
	rbtFiles.append(file)

FIRST_READ = True
fp = open('{}/{}concat'.format(rbtLocation,rbtPattern), 'wb')


for fileR in rbtFiles:
	with open(fileR, 'rb') as file:
		print(fileR)
		l = int(file.read(1))
		k = b''
		while True:
			byte = file.read(1)
			if byte == b"\n":
				break
			k += byte
		k = int(k)

		if FIRST_READ:
			fp.write(bytes(str(l), 'UTF-8'))
			fp.write(bytes(str(k), 'UTF-8'))
			fp.write(bytes("\n", 'UTF-8'))
			FIRST_READ = False

		initialPwd = b''
		lastPwd = b''
		while True:
			byte = file.read(1)
			if len(initialPwd) < l:
				initialPwd += byte
			elif len(lastPwd) < l:
				lastPwd += byte

			if len(lastPwd) == l:
				fp.write(initialPwd)
				fp.write(lastPwd)
				initialPwd = b''
				lastPwd = b''

			if byte == b"":
				break
fp.close()