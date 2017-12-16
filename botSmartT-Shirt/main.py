
import os
import pycurl
import sys
import shutil
import hashlib
import requests
import threading
import telebot
import cPickle as pickle
from firebase.firebase import FirebaseApplication, FirebaseAuthentication
import time
import datetime
import numpy as np
import pylab as pl

SECRET = 'uRwL0OL0ICuAPBw7HKn0rgyMmCQnRrrp3MKnCfau'
DSN = 'https://testw-8b1b6.firebaseio.com/'
EMAIL = 'ibarbech@alumnos.unex.es'
TOKEN = '508897766:AAGBYHFEFhXaUGo4vDFLwG39sSSEedK9v9E'

files=[
	"https://www.unex.es/organizacion/servicios-universitarios/servicios/servicio_recursos_humanos/funciones/convpci",
]
bot = telebot.TeleBot(TOKEN)
list_chat_id = []
data = {}


try:
	with open("idchats.txt", 'rb') as fichero:
	    list_chat_id = pickle.load(fichero)
except Exception as e:
	with open("idchats.txt", 'wb') as fichero:
		pickle.dump(list_chat_id, fichero,0)


@bot.message_handler(commands=['start', 'help'])
def handle_start_help(message):
    if list_chat_id.count(message.from_user.id) is 0:
        print "anadiendo"
        list_chat_id.append(message.from_user.id)
        with open("idchats.txt", 'wb') as fichero:
            pickle.dump(list_chat_id, fichero,0)

    bot.reply_to(message, "Howdy, how are you doing?\n\t/help\n\t/exit\n\t/getGraphic")

@bot.message_handler(commands=['exit'])
def handle_exit(message):
    list_chat_id.remove(message.from_user.id)
    with open("idchats.txt", 'wb') as fichero:
        pickle.dump(list_chat_id, fichero,0)
        bot.reply_to(message, "Hasta la proxima!!!")

def keyToTimeStamp(key):
        PUSH_CHARS ='-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz'
        idString = key[0:8]
        time = 0
        for letter in idString:
                time = time * 64 + PUSH_CHARS.index(letter)

        return time

@bot.message_handler(commands=['getGraphic'])
def handle_getGraphic(message):
	dictGraph={}
	for key in data.keys():
		timestamp = keyToTimeStamp(key)
		date = time.localtime(timestamp/1000)
		if datetime.date(date.tm_year,date.tm_mon,date.tm_mday) in dictGraph.keys():
			dictGraph[datetime.date(date.tm_year,date.tm_mon,date.tm_mday)]+=1
		else:
			dictGraph[datetime.date(date.tm_year,date.tm_mon,date.tm_mday)]=1
	keys = dictGraph.keys()
	sorted(dictGraph)
	x = []
	y = []
	nday=0
	prev_key = None
	for act_key in keys:
		y.append(dictGraph[act_key])
		dif=0
		if prev_key is not None:
			dif = abs((act_key - prev_key).days)
			print dif, act_key, prev_key
		prev_key = act_key
		nday+=dif
		x.append(nday)
	pl.bar(x, y, facecolor='green', alpha=0.75)
    # pl.show()
	pl.savefig('/tmp/photo.png')
	photo = open('/tmp/photo.png', 'rb')
	for chat_id in list_chat_id:
		bot.send_photo(chat_id, photo)
		bot.send_photo(chat_id, "FILEID")
	pass

@bot.message_handler(func=lambda message: True)
def echo_all(message):
    bot.reply_to(message, message.text)

def worker(bot):
    bot.polling()

if __name__ == '__main__':
	t = threading.Thread(target=worker, args=(bot,))
	t.start()

	authentication = FirebaseAuthentication(SECRET,EMAIL,True)
	firebase = FirebaseApplication(DSN, authentication)
	try:
		with open("data.txt", 'rb') as fichero:
			data = pickle.load(fichero)
	except Exception as e:
		with open("data.txt", 'wb') as fichero:
			pickle.dump(data, fichero,0)
	while True:
		result = firebase.get('/', None)
		if len(data.keys()) != len(result.keys()):
			data = result
			for chatid in list_chat_id:
				bot.send_message(chatid, "Ponte recto")
