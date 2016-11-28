#!/usr/bin/env python

from socket import *
import sys
import os

MAXSIZE = 2048


#send message to server
def sendMsg(sock, data):
	#print data
	sock.sendall(data)

#receive data from server
def recvMsg(sock, size):
	buf = ''
	totalRead = 0
	#print size
	if size == MAXSIZE:
		data = sock.recv(MAXSIZE)
		return data
	while totalRead + 1 < size:
		data = sock.recv(MAXSIZE)
		#print data
		totalRead = totalRead + len(data)
		#print totalRead + 1
		buf = buf + data	
	return buf


#print directory recieved from server then close socket
def recvDir(sock):
	direct = recvMsg(sock, MAXSIZE)
	print direct
	sock.close()

	
#receive file from server and save to directory then close socket
#stack overflow question 678236
def recvfile(sock, filename, host, dPort):
	#output message
	ofilename = filename
	num = 0
	filenum = ''
	filename = os.path.splitext(filename)[0]
	#handle file exists error
	while os.path.isfile('./' + filename + '.txt'):
		#print "exists"
		num = num + 1
		filenum = str(num)
		filename = filename + filenum
	filename = filename + '.txt'
	#print filename
	outfile = open(filename, 'w+')
	size = recvMsg(sock, MAXSIZE)
	#print repr(size) + '\n'
	size = size.rstrip('\0')
	size = int(size)
	sendMsg(sock, "OK")
	print "Receiving \"{0}\" from {1}: {2}".format(ofilename, host, dPort)
	data = recvMsg(sock, size + 1);
	#print data
	outfile.write(data)
	outfile.close()	
	sock.close()
	print "File transfer complete"


# make initial connection with server
def initiateContact(port, host, dPort, command):
	#cast port
	port = int(port)
	#change host to proper set up
	hostname = host + ".engr.oregonstate.edu"
	#make connection with waiting server
	sock = socket(AF_INET, SOCK_STREAM)
	sock.connect((hostname, port))
	#send data port to server	
	sendMsg(sock, dPort)
	#get confimation	
	validPort = recvMsg(sock, MAXSIZE)
	if validPort == "OK":
		#send command to server
		sendMsg(sock, command)	
		#get confirmation	
		com = recvMsg(sock, MAXSIZE)
		#check if valid
		if com == "OK":
			#return sock
			return sock
		elif com == "FAIL":
			#
			print "Error: Invalid Command from {0}:{1}".format(host, port)
			sock.close()
			sys.exit(2)

	
#check if file is valid and get directory
def makeRequest(sock, dsock, command, host, port, dPort, filename):
	if command == "-g":
		#send file name
		sendMsg(dsock, filename)
		#get error from original socket
		validfile = recvMsg(dsock, MAXSIZE)
		#get confirmation
		if validfile == "OK":
			recvfile(dsock, filename, host, dPort)
			dsock.close()
		elif validfile == "FAIL":
			dsock.close()
			return -1
	elif command == "-l":
		print "Receiving directory structure from {0}:{1}".format(host, dPort)
		directory = recvDir(dsock)
		dsock.close()

#check if command line command is valid
def validateCommand(command):
	if command != '-l' and command != '-g':
		print "Error: Invalid Command"
		sys.exit(2) 


def validatePort(number):
	try:
		num = int(number)
	except ValueError:
		print("Use a Valid Port")
		sys.exit(2)


def main(argv):

	if len(sys.argv) < 5:
		print "USAGE: <SERVER HOST> <SERVER PORT> <COMMAND> [FILENAME] <DATA PORT>"
		sys.exit(2)		
	
	host = sys.argv[1] 	
	port = sys.argv[2]
	command = sys.argv[3]

	if len(sys.argv) == 5:
		filename = None
		dPort = sys.argv[4]	
	elif len(sys.argv) == 6: 
		filename = sys.argv[4]
		dPort = sys.argv[5]
		#print filename

	#validate port and command inputs
	validatePort(port)
	validatePort(dPort)
	validateCommand(command);

	#make inital connection
	sock = initiateContact(port, host, dPort, command)
	
	#make data connection	
	dsock = socket(AF_INET, SOCK_STREAM)
	dPort = int(dPort)
	
	dsock.bind(('', dPort))
	dsock.listen(1)
	#print "Data Connection open"
	
	newsock, addr = dsock.accept()
	#print "connection made from address: {0}".format(addr)
	'''
	dsock.connect((host, dPort))
	#dsock.setblocking(0)
	'''
	#get file
	valid = makeRequest(sock, newsock, command, host, port, dPort, filename)
	if valid == -1:
		print "{0}:{1} says FILE NOT FOUND".format(sys.argv[1], port)
		dsock.close()
		sock.close()
		sys.exit(2)
	
	newsock.close()
	dsock.close()
	sock.close()


if __name__ == "__main__":
	main(sys.argv[1:])



 
