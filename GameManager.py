import socket
import threading
import sys
import json
import time
from player import *
from AI import *
from card import *

#########################################################
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
SERVER_IP = '140.113.123.225'
SERVER_PORT = 7975
SERVER_ADDRESS = (SERVER_IP, SERVER_PORT)
sock.connect(SERVER_ADDRESS)
######################################################### set the net connection
msg = ""			# the messenge you send to server
rev = ""			# the messenge you receive from server
player_id = []
game_id = ""
lock = threading.Lock()
#########################################################
class myThread(threading.Thread):
	def __init__(self, lock, No):
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.connect(SERVER_ADDRESS)
		threading.Thread.__init__(self)
		self.No = No
		self.lock = lock

	def run(self):
		self.lock.acquire()
		if (actionList[self.No] == "create"):
			self.connect("create")
			actionList[self.No] = "listen"
			actionList[2] = "join"
			actionList[3] = "join"
			actionList[4] = "join"
		elif (actionList[self.No] == "join"):
			if (game_id == ""):
				pass
			else:
				self.connect("join")
				actionList[self.No] = "listen"
		elif (actionList[self.No] == "listen"):
			self.listen()
		else:
			pass
		self.lock.release()

	def listen(self):
		global actionList
		revList = []
		rev = self.sock.recv(2048)
		if ("}{" in rev):
			revList = rev.split("}{")
			for i in range(len(revList)):
				if (revList[i][-1] != '}'):
					revList[i] = revList[i] + "}"
				if (revList[i][0] != '{'):
					revList[i] = "{" + revList[i]
				while (revList[i].count('{') != revList[i].count('}')):
					if (revList[i].count('{') > revList[i].count('}')):
						revList[i] = revList[i] + "}"
					else:
						revList[i] = "{" + revList[i]
				print revList[i]
		else:
			revList.append(rev)
		while (len(revList) != 0):
			print "rev", "(", len(revList), ")", revList[0]
			rev = json.loads(revList[0])
			if ('msg' in rev):
				print "player", self.No, "receives:", rev['msg']
			if ('player_id' in rev and 'game_id' in rev):
				print self.No, rev['player_id'], rev['game_id']
				global game_id
				game_id = rev['game_id']
			if ('action' in rev):
				if (rev['action'] == 'get'):
					print rev['hand']
					playerAction = player(rev['hand'], rev['discard'], rev['action'])
					self.playerMove(playerAction)

			revList.pop(0)

	def playerMove(self, playerAction):
		if (playerAction == "draw"):
			msg = json.dumps({"action":"game", "command":"draw", "game_id":game_id})
			print "player", self.No, "send", msg
			self.sock.sendall(msg)

	def connect(self, action):
		global game_id
		if (action == "create"):
			msg = json.dumps({"action":"room", "command":"create"})
			print msg
			self.sock.sendall(msg)
		elif (action == "join"):
			msg = json.dumps({"action":"room", "command":"join", "game_id":game_id})
			print msg
			self.sock.sendall(msg)

	def disconnect(self):
		self.sock.close()
		print "player", self.No, "is disconnected."

playerList = [myThread(lock, 1), myThread(lock, 2), myThread(lock, 3), myThread(lock, 4)]
actionList = [0, "create", "", "", ""]

while (1):
	playerList[0].run()
	time.sleep(0.1)
	playerList[1].run()
	time.sleep(0.1)
	playerList[2].run()
	time.sleep(0.1)
	playerList[3].run()
	time.sleep(0.1)
	input = raw_input('next round')

for player in playerList:
	player.disconnect()

print "end of GameManager.py"
