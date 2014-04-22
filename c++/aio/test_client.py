#!/usr/bin/python
#coding=utf-8

import socket

def main():
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect(("localhost", 8701))
	print "send"
	sock.send("test")
	print "recv"
	data = sock.recv(512)
	print data
	sock.close()

if __name__ == '__main__':
	main()
