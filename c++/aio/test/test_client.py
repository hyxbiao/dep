#!/usr/bin/python
#coding=utf-8

import socket
import threading
import struct

class Test(threading.Thread):
	def __init__(self, name):
		super(Test, self).__init__(name = name)

	def run(self):
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.connect(("localhost", 8701))
		body = 'long long ago'
		content_head = struct.pack('iI', 1, len(body))
		print self.name, "send", body
		sock.send(content_head)
		sock.send(body)
		#print self.name, "recv"
		data = sock.recv(512)
		version, body_len = struct.unpack("iI", data[:8])
		print self.name, "recv", data[8:]
		sock.close()
		pass

def main():
	num = 10
	threads = []
	for i in range(num):
		name = 'thread%d' % i
		thread = Test(name)
		thread.start()
		threads.append(thread)

	for i in range(num):
		threads[i].join()

if __name__ == '__main__':
	main()
