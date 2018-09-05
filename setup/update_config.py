#!/usr/bin/python

# Updates the configuration in the json to the database

import json
import sqlite3
import sys

if len(sys.argv) > 1:
    filepath = str(sys.argv[1])
else:
    sys.exit('Please enter as argument a filepath in the folder Mission')

print(filename)
try:
    cfg = json.load(open(filename))
except FileNotFoundError:
    sys.exit('Error to open the file.\nPlease enter as argument a filepath.')

conn = sqlite3.connect('boat.db')
db = conn.cursor()

for table in cfg:
    data = cfg[table]
    setstr = ''
    keystr = ''
    valstr = ''
    for key, value in cfg[table].items():
        if isinstance(value, str):
            value = '"' + value + '"'
        else:
            value = str(value)
        if (setstr == ''):
            setstr = key + ' = ' + value
            keystr = key
            valstr = value
        else:
            setstr = setstr + ', ' + key + ' = ' + value
            keystr = keystr + ', ' + key
            valstr = valstr + ', ' + value
    try:
        db.execute('SELECT count(*) FROM ' + str(table) + ';')
    except sqlite3.OperationalError:
        sys.exit('Error to retrieve the tables.\nCheck if the selected file \''+filename+'\' correspond to the current DataBase configuration')
    count = db.fetchone()[0]
    if count == 0:
        db.execute('INSERT INTO ' + str(table) + ' (' + keystr +
                   ') VALUES (' + valstr + ');')
    else:
        db.execute('UPDATE ' + str(table) + ' SET ' +
                   setstr + ' WHERE ID = 1;')
conn.commit()
db.close()

print('Successful load of new configurations from the file: ' + filepath)

