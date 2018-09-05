#!/usr/bin/python3

# Python script to update/add waypoints to the database with data from a json

import json
import sqlite3
import sys
from collections import OrderedDict

if len(sys.argv) > 1:
    filepath = str(sys.argv[1])
else:
    sys.exit('Please enter as argument a filepath in the folder Mission')

print(filename)
try:
    waypoints = json.load(open(filepath), object_pairs_hook=OrderedDict)
except FileNotFoundError:
    sys.exit('Error to open the file.\nPlease enter as argument a filepath in the folder Mission')

conn = sqlite3.connect('boat.db')
db = conn.cursor()

db.execute('DELETE FROM currentMission')
for wp in waypoints:
    keystr = 'id'
    valstr = str(wp)
    for key, value in waypoints[wp].items():
        value = str(value)
        keystr = keystr + ', ' + key
        valstr = valstr + ', ' + value
    db.execute('INSERT INTO currentMission (' + keystr +
               ') VALUES (' + valstr + ')')
conn.commit()
db.close()

print('Successful load of new waypoints from the file: ' + filepath)
