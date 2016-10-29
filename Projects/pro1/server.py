#!/usr/bin/python

from socket import *
import sys

MAXDATASIZE = 500

#https://docs.python.org/3.3/howto/sockets.html

#send msg function
def sendmsg(msg, sock, addr):
	totalsent = 0
	while totalsent < len(msg):
		sent = sock.send(msg[totalsent:])
		if sent == 0:
			print "Connection with {} is closed".format(addr)
			sock.close()
		totalsent = totalsent + sent

#recieve message fxn
def recmsg(sock, size):
	msg = ''
	msg = sock.recv(size)
	return msg 


def main(argv):

	#check if there is the correct number of arguments in the command line
	if len(sys.argv) < 2:
		print 'Usage: server.py <PORTNUMBER>'
		sys.exit(2)
	elif len(sys.argv) > 2:
		print 'Usage: server.py <PORTNUMBER>'
		sys.exit(2)

	#get port from command line args
	port= int(sys.argv[1])

	handle = ''	
	#get user handle
	while len(handle) == 0 or len(handle) > 10:
		handle = raw_input('Input 10 Character Server Handle: ')
	
	# create listening TCP socket
	serverSocket = socket(AF_INET, SOCK_STREAM)
	serverSocket.bind(('', port))
	
	# listen for incoming TCP requests
	serverSocket.listen(1)
	print 'The Server is ready to receive'
	#wait for accept() on incoming requests
	
	while 1:
		connectionSocket, addr = serverSocket.accept()
		#read bytes from socket
		print "connection made from address {}".format(addr)
		#get handle
		
		b_handle = recmsg(connectionSocket, 1024)
		#strip trailing new line
		b_hand = b_handle.rstrip('\n')
		print "You are speaking to: " + b_hand
			
		#prompt for recieved messages
		recprompt = b_hand + ">"
				
		#send handle to client
		connectionSocket.send(handle)

		while 1:
			#receive message
			recmessage = connectionSocket.recv(MAXDATASIZE)
			if recmessage:
				recmessage = recmessage.rstrip()
				print recprompt + recmessage	
			
			#send message
			sentmessage = raw_input(">")
			if sentmessage == '\quit':
				print "Goodbye"
				break
			sendmsg(sentmessage, connectionSocket, addr)
			
			#close connection
		connectionSocket.close()
		print "Connection with {} is closed".format(addr)

if __name__ == "__main__":
	main(sys.argv[1:])

