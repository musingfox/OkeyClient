#!/usr/bin/env python2.7

import socket
import json

sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

sock.connect(("140.113.235.151",7975))

data = {"command":"game","setting":{"log":"verbose"}}

sock.sendall(json.dumps(data))
sock.recv(2048)

sock.sendall(json.dumps({"player":1,"action":"hand"}))
hand = json.loads(sock.recv(2048))
print(hand['cards']['hand'])

sock.sendall(json.dumps({"player":1,"action":"take","from":"deck"}))
takeCard = json.loads(sock.recv(2048))
print("\n\n\nTake:\n"+str(takeCard['card']))

sock.sendall(json.dumps({"player":1,"action":"throw","cards":hand['cards']['hand']}))
print(sock.recv(2048))
print("\n\n")
sock.sendall(json.dumps({"player":2,"action":"hand"}))
print(sock.recv(2048))

