#coding:utf-8
import socket
import cv2
import numpy as np
import xml.dom.minidom
import datetime
import sys
import struct
import commands
import os

workdir = os.getcwd()
innerhost = "localhost"
innerport = 9233
SAVE_IMG = 0


class Model(object):
    def __init__(self, num=0):
        self.name = ''
        self.width = 227
        self.height = 277
        self.channel = 3
        self.type = ''
        self.labels = []
        self.tf_param = []  #pred, x, keep_prob
        self.initmodel(num)
        self.initlabel()
    def initmodel(self, num):
        dom = xml.dom.minidom.parse('config.xml')
        root = dom.documentElement
        m_mdlnum = root.getElementsByTagName('modelnum')
        itemlist = root.getElementsByTagName('model')
        model_content = itemlist[num]
        self.name = str(model_content.getAttribute("name"))
        self.type = str(model_content.getAttribute("type"))
        self.model_path = str(model_content.getAttribute("path"))
        if self.type == 'caffe':
            import caffe
            caffe.set_mode_gpu()
            proto_data = open(self.model_path + 'mean.binaryproto', 'rb').read()
            temp_a = caffe.io.caffe_pb2.BlobProto.FromString(proto_data)
            img_mean = caffe.io.blobproto_to_array(temp_a)[0]
            self.net = caffe.Net(self.model_path + 'deploy.prototxt', self.model_path + 'model.caffemodel', caffe.TEST)
            tm1, self.channel, self.width, self.height = self.net.blobs['data'].data.shape
            img_mean = np.transpose(img_mean, [1, 2, 0])
            img_mean = cv2.resize(img_mean, (self.width, self.height))
            self.mean = np.transpose(img_mean, [2, 0, 1])                  
            print 'caffe done!'
        elif self.type == 'tensorflow':
            import tensorflow as tf
            ckpt = tf.train.get_checkpoint_state(self.model_path)
            saver = tf.train.import_meta_graph(ckpt.model_checkpoint_path + '.meta')
            self.pred = tf.get_collection("pred")[0]
            self.x = tf.get_collection("x")[0]
            self.keep_prob = tf.get_collection("keep_prob")[0]
            gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction = 0.1)
            self.sess = tf.Session(config = tf.ConfigProto(gpu_options = gpu_options))
            saver.restore(self.sess, ckpt.model_checkpoint_path)
            print 'tf done!'
        elif self.type == "test":
            pass
        else:
            print 'can not recognized the frame!'

    def initlabel(self):
        try:
            f = open(self.model_path + 'labels.txt', 'r')
            while True:
                line = f.readline()
                line = line.strip('\n')
                # if SAVE_IMG:
                    # commands.getstatusoutput('mkdir -p pic/' + model.name + '/' + m_date + '/' + line)
                if line:
                    self.labels.append(line)
                else:
                    break
            f.close()
        except:
            self.labels.append("test1")
            self.labels.append("test2")
    def predict(self, img):
        if self.type == 'caffe':
            img_in = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)
            img_in = np.transpose(img_in, [2, 0, 1])
            img_in = img_in.astype(np.float32)
            img_in -= self.mean
            self.net.blobs['data'].data[...] = [img_in]
            output = self.net.forward()
            predictions = output['prob']
        elif self.type == 'tensorflow':
            img_in = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)
            img_in = img_in.astype(np.float32)
            img_in /= 255
            predictions = self.sess.run(self.pred, feed_dict={self.x: [img_in], self.keep_prob: 1.})
        else:
            predictions = (2.2, 1.6)
        m_rlt = self.labels[np.argmax(predictions)]
        return m_rlt


class FpsCheck(object):
    def __init__(self, tickt=10):
        self.stime = datetime.datetime.now()
        self.pronum = 0
        self.tick = tickt
        self.fps = 0

    def process(self):
        self.pronum += 1
        if self.pronum >= self.tick:
            fps = (self.tick)/((datetime.datetime.now() - self.stime).total_seconds())
            self.pronum = 0
            self.stime = datetime.datetime.now()
            self.fps = fps
        return self.fps


class imgSaver:
    def __init__(self):
        self.m_date = str(datetime.datetime.now().strftime("%Y-%m-%d-%H:%M:%S"))
        self.ca_num = 0
    def save(self, issave):
        if(0 == self.ca_num%2000):
            picFolder = str(self.ca_num)
        filename = workdir + \
                   '/pic/' + \
                   m_model.name + \
                   '/' + self.m_date + \
                   '/' + m_rlt + \
                   '/' + picFolder + \
                   '/' + str(self.ca_num) + \
                   '.jpg'
        if issave:
            commands.getstatusoutput('mkdir -p pic/' + m_model.name + '/' + self.m_date + '/' + m_rlt + '/' + picFolder)
            cv2.imwrite(filename, img)
        self.ca_num += 1
        return filename


def receiveimg(s, imglen):
    data = ''
    recv_size = 0
    while recv_size < imglen:
        if imglen - recv_size > 10240:
            data += s.recv(10240)
        else:
            data += s.recv(imglen - recv_size)
        recv_size = len(data)
        if data == 'are you there?':
            s.sendall('yes')
            data = ''
    return data

mn = int(sys.argv[1])
m_model = Model(mn)
imglen = m_model.width * m_model.height
s = socket.socket()
s.connect((innerhost, innerport))
data = struct.pack('=i'+str(len(m_model.name))+'s3i',
                   len(m_model.name),
                   m_model.name,
                   m_model.width,
                   m_model.height,
                   m_model.channel)
s.sendall(data)
data = s.recv(20)
if data == 'connect secceed':
    print 'secceed'
fps = FpsCheck()
saver = imgSaver()
while True:
    try:
        data = receiveimg(s, imglen)
    except:
        break
    img = np.fromstring(data, dtype=np.uint8)
    img = img.reshape(m_model.height, m_model.width)
# {outputdata m_rlt:result nfps: speed filename: store dir
    m_rlt = m_model.predict(img)
    nfps = fps.process()
    filename = saver.save(SAVE_IMG)
# }
    len_m_rlt = len(m_rlt)
    len_filename = len(filename)
    data = struct.pack('=3i' + str(len_m_rlt) + 's' + str(len_filename) + 's',
                       len_m_rlt,
                       len_filename,
                       nfps, m_rlt,
                       filename)
    s.sendall(data)
s.close()
