# _*_coding:utf-8_*_
import socket
import threading
import struct
import cv2
import time
import Queue
import logging

QMAX = 500
logger = logging.getLogger('TOEClogger')
class Mylist(list):
    def __init__(self):
        list.__init__(self)
        self.mutex = threading.Lock()

    def append(self, p_object):
        self.mutex.acquire()
        list.append(self, p_object)
        self.mutex.release()

    def clean(self):
        self.mutex.acquire()
        for model in self:
            if model.flag == 0:
                self.remove(model)
                break
        self.mutex.release()


class NoModelResource(Exception):
    pass


class ModelPro:
    def __init__(self, conn):
        self.conn = conn
        self.initconnect(conn)
        self.qimpro = Queue.Queue()
        self.flag = 1
        self.fps = 0
        sthread = threading.Thread(target=self.imgpro)
        sthread.setDaemon(True)
        sthread.start()

    def initconnect(self, conn):
        data = conn.recv(4)
        num = struct.unpack('i', data)[0]  # 字符字节数
        data = conn.recv(num)
        self.name = struct.unpack(str(num) + 's', data)[0]
        data = conn.recv(12)
        self.width, self.height, self.chanel = struct.unpack('3i', data)

    def imgpro(self):
        try:
            self.conn.sendall("connect secceed")
        except:
            self.conn.close()
            self.flag = 0
            return
        while True:
            try:
                tmp = self.qimpro.get(block=True, timeout=1)
                img_in = tmp[0]
                Qpro = tmp[1]
                img_in = cv2.resize(img_in, (self.width, self.height))
                self.conn.sendall(img_in.data.__str__())
                data = self.conn.recv(128)
                len1, len2, self.fps = struct.unpack("3i", data[:12])
                m_rlt, adrr = struct.unpack(str(len1) + 's' + str(len2) + 's',data[12:])
                Qpro.put((m_rlt, adrr))
            except Queue.Empty:
                self.conn.sendall('are you there?')
                data = self.conn.recv(3)
		self.fps = 0
                if data == 'yes':
                    continue
                else:
                    break
            except:
                logger.exception("dpserve exception")
                break
####################################################################加入返回列队
        self.conn.close()
        self.flag = 0
        while True:
            try:
                tmp = self.qimpro.get(block=False)
                conc = tmp[1]
                conc.send("failed")
                conc.close()
            except:
                break
        return


class ModelManage:
    def __init__(self, host='0.0.0.0', port=9231):
        self.host = host
        self.port = port
        self.modellist = Mylist()
        sthread = threading.Thread(target=self.cnn_add)
        sthread.setDaemon(True)
        sthread.start()
        sthread = threading.Thread(target=self.cnn_destroy)
        sthread.setDaemon(True)
        sthread.start()

    def cnn_add(self):
        s = socket.socket()
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((self.host, self.port))
        s.listen(1)
        while True:
            conn, addr = s.accept()
            try:
                self.modellist.append(ModelPro(conn))
            except:
                continue

    def cnn_destroy(self):
        while True:
            time.sleep(0.5)
            self.modellist.clean()

    def put(self, model_name, img, proQ):
        self.modellist.mutex.acquire()
        try:
            for model in self.modellist:
                if model.name[:4] == model_name[:4]:
                    if model.qimpro.qsize() < QMAX:
                        model.qimpro.put((img, proQ))
                        return True
        except:
            logger.exception('put exception')
        finally:
            self.modellist.mutex.release()
        raise NoModelResource("No Model")

    def checkload(self):
        qlist = list()
        self.modellist.mutex.acquire()
        for model in self.modellist:
            qlist.append((model.name, model.qimpro.qsize(), model.fps))
        self.modellist.mutex.release()
        return qlist


