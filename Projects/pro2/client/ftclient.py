#!usr/bin/python

from socket import *
import sys

MAXSIZE = 2048

def sendMsg(sock, data):
	sock.sendall(data)


def recvMsg(sock, size):
	buffer = []
	totalRead = 0
	if size == MAXSIZE:
		data = sock.recv(MAXSIZE)
		return data
	while totalRead < size:
		data = sock.recv(min(size-totalRead, MAXSIZE))
		if data == b'':
			print "Error"
		buffer.append(data)
		totalRead = totalRead + len(data)
	return b''.join(data)

def recvDir(sock):
	direct = recvMsg(sock, MAXSIZE)
	print direct
	

def recvfile(sock, filename):
	outfile = open(filename, 'w+')
	#size = recvMsg(sock, MAXSIZE)
	#print repr(size) + '\n'
	#sendMsg(sock, "OK")
	data = recvMsg(sock, MAXSIZE)
	#print data
	outfile.write(data)
	outfile.close()	
	print "File transfer complete"


def main(argv):

	if len(sys.argv) < 5:
		print "USAGE: <SERVER HOST> <SERVER PORT> <COMMAND> [FILENAME] <DATA PORT>"
		sys.exit(2)		
	
	host = sys.argv[1] + ".engr.oregonstate.edu"
	port = sys.argv[2]
	command = sys.argv[3]

	if len(sys.argv) == 5:
		dPort = sys.argv[4]	
	elif len(sys.argv) == 6: 
		filename = sys.argv[4]
		dPort = sys.argv[5]
		#print filename

	#print host
	#print port
	#print command
	#print dPort
	#if filename:
	#	print filename

	port = int(port)
	sock = socket(AF_INET, SOCK_STREAM)
	sock.connect((host, port))
	#send data port to server
	sendMsg(sock, dPort)
	#check if it worked
	validPort = recvMsg(sock, MAXSIZE)
	if validPort == "OK":
		#send selected command to server
		sendMsg(sock, command)
		com = recvMsg(sock, MAXSIZE)
		#verify command
		if com == "OK":
			#print "VALID COMMAND"
			#open new server
			dsock = socket(AF_INET, SOCK_STREAM)
			dPort = int(dPort)
			dsock.connect((host, dPort))
			#dsock.setblocking(0)
			#get file
			if command == "-g":
				#send file name
				sendMsg(dsock, filename)
				#get error from original socket
				validfile = recvMsg(dsock, MAXSIZE)
				#get confirmation
				if validfile == "OK":
					recvfile(dsock, filename)
				elif validfile == "FAIL":
					print "{0}:{1} says FILE NOT FOUND".format(sys.argv[1], port)
					dsock.close()
					sock.close()
					sys.exit(2)
			elif command == "-l":
				print "Receiving directory structure from {0}:{1}".format(sys.argv[1], dPort)
				directory = recvDir(dsock)
			dsock.close()
		elif com == "FAIL":
			print "INVALID COMMAND"
			sock.close()
			sys.exit(2)
	sock.close()



if __name__ == "__main__":
	main(sys.argv[1:])



 
