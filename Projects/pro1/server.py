#!/usr/bin/python

from socket import *
import sys


def main(argv):

	#check if there is the correct number of arguments in the command line
	if len(sys.argv) < 2:
		print 'server.py <PORTNUMBER>'
		sys.exit(2)
	elif len(sys.argv) > 2:
		print 'server.py <PORTNUMBER>'
		sys.exit(2)

	port= sys.argv[1]
	print 'port number is', port 


	#get server port from user
	serverPort = int(port)
	
	#get user handle
	handle = raw_input('Input User Handle: ')

	#print "User handle is: %s" %handle
	
	# create listening TCP socket
	serverSocket = socket(AF_INET, SOCK_STREAM)
	serverSocket.bind(('', serverPort))
	

	# listen for incoming TCP requests
	serverSocket.listen(1)
	print 'The Server is ready to recieve'
	#wait for accept() on incoming requests
	
	sys.exit()
	while 1:
		connectionSocket, addr = serverSocket.accept()
		#read bytes from socket
		sentence = connectionSocket.recv(1024)
		capitalizedSenntence = sentence.upper()
		connectionSocket.send(capitalizedSentence)
		#close connection
		connectionSocket.close()



if __name__ == "__main__":
	main(sys.argv[1:])

