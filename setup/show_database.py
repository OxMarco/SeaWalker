#!/usr/bin/python

import sqlite3 as lite

conn = lite.connect('boat.db')
cur = conn.cursor()

def show_all():
    with conn:
        cur.execute("SELECT * FROM config_wingsail_control")
        print(cur.fetchall())

show_all()
