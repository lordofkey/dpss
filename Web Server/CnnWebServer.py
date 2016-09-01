# coding: utf-8
from flask import Flask
from flask import render_template
import socket
import struct
import json
import pymongo

cnnhost = "172.1.10.134"
cnnport = 10102
dbhost = "172.1.10.134"
dbport = 27017
app = Flask(__name__)


@app.route('/')
def hello_world():
    return render_template("index.html")

@app.route('/getstatus')
def getstatus():
    res = {}
    try:
        s = socket.socket()
        s.connect((cnnhost, cnnport))
        data = s.recv(4)
        num,  = struct.unpack('i',data)

        for i in range(num):
            snum,  = struct.unpack('i', s.recv(4))
            name, value = struct.unpack('='+str(snum)+'si', s.recv(snum+4))
            if name == "receivelist":
                name = '外部连接负载'
            res[name] = value
    except:
        res["CNNSERVER 连接失败"] = 0
    js = json.dumps(res, ensure_ascii=False)
    return js

@app.route('/getrecord')
def getrecord():
    client = pymongo.MongoClient(dbhost, dbport)
    num =  client.deepldb.test.count()
    sernams = client.deepldb.test.distinct("sername")
    res = {}
    res["总记录数目"] = num
    for sername in sernams:
        num =  client.deepldb.test.find({"sername": sername}).count()
        sername = sername.encode()
        res[sername] = num
    js = json.dumps(res, ensure_ascii=False)
    return js

if __name__ == '__main__':
    app.run()
