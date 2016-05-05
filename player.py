import json
from AI import *
from card import *

hand = []

memory = [[2,2,2,2,2,2,2,2,2,2,2,2,2,2],
		  [2,2,2,2,2,2,2,2,2,2,2,2,2,2],
		  [2,2,2,2,2,2,2,2,2,2,2,2,2,2],
		  [2,2,2,2,2,2,2,2,2,2,2,2,2,2]]

No = 0
ID = ""

def hand_clear():
	del hand[0:len(hand)]

def draw_card(card):
	hand.append(card)

def getCardInHand(item):
	color = str(item['color'])
	number = int(item['number'])
	if (color != 'empty' and number != -1):
		draw_card(card(number, color))
		writeIntoMemory(card(number, color))

def writeIntoMemory(card):
	if (card.color == 'black'):
		memory[0][card.number]-=1
	elif (card.color == 'blue'):
		memory[1][card.number]-=1
	elif (card.color == 'red'):
		memory[2][card.number]-=1
	elif (card.color == 'yellow'):
		memory[3][card.number]-=1

##########################################################

def player(rev, card, action, memory):
	for line in rev:
		for index in line:
			getCardInHand(index)
	card = card(card['number'], card['color'])

	AI_response = AI(hand, card, action)
	return AI_response
