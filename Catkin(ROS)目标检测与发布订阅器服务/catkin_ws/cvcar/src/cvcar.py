#!/usr/bin/env python
# -*- coding:utf-8 -*-
import os
from Tkinter import *
import cv2
import time
from PIL import Image
from PIL import ImageTk
import roslib;
roslib.load_manifest('cvcar')
import rospy
from std_msgs.msg import String
from geometry_msgs.msg import Twist
import multiprocessing
import subprocess 
from subprocess import Popen,PIPE
import threading

rospy.init_node('cvcar', anonymous=True)
publisher = rospy.Publisher('car_cmd', Twist, queue_size=1)

def sendcmd(x, y):
	cmd = Twist()
	cmd.linear.x  = x
	cmd.linear.y  = y
	cmd.linear.z  = 0.0
	cmd.angular.x = 0.0
	cmd.angular.y = 0.0
	cmd.angular.z = 0.0
	publisher.publish(cmd)
	
class Teleop:
	def __del__(self):
		os.system('sudo pkill rosout')
		
t = Teleop()

def callback(data):
	rospy.loginfo("I heard %s", data.data)
	tag = data.data
	tags = ['cat', 'bicycle', 'sofa', 'dog', 'train', 'car']
	if tag not in tags:
           return
	sendcmd(1, tags.index(tag) + 1)
	
def listener():
	rospy.Subscriber("chatter", String, callback)
	rospy.spin()

def upClick():
	sendcmd(2, 1)
def leftClick():
	sendcmd(2, 2)
def rightClick():
	sendcmd(2, 3)
def backClick():
	sendcmd(2, 4)
def spaceClick():
	sendcmd(2, 5)
def upKey(event):
	upClick()
def leftKey(event):
	leftClick()
def rightKey(event):
	rightClick()
def backKey(event):
	backClick()
def spaceKey(event):
	spaceClick()
	
def openVideo():
	os.system('rosrun ros_caffe ros_caffe_test  &')
def videoClick():
	t = threading.Thread(target=openVideo)
	t.setDaemon(True)
	t.start()
	
if __name__ == '__main__':
	listenThread=threading.Thread(target=listener)
	listenThread.setDaemon(True)
	listenThread.start()

	root = Tk()
	
	f = Frame(root, height=256, width=256)
	f.bind('<Up>',   upKey)
	f.bind('<Left>', leftKey)
	f.bind('<Right>',rightKey)
	f.bind('<Down>', backKey)
	f.bind('<space>',spaceKey)
	f.focus_set()
	f.pack()
	
	top         = Button(f, text='Up',    width=8, height=8, command=upClick)
	left        = Button(f, text='Left',  width=8, height=8, command=leftClick)
	right       = Button(f, text='Right', width=8, height=8, command=rightClick)
	back        = Button(f, text='Back',  width=8, height=8, command=backClick)
	space       = Button(f, text='Stop',  width=8, height=8, command=spaceClick)
	videoButton = Button(f, text='Video', width=4, height=4, command=videoClick)
	
	space.grid(row=1, column=1)
	top.grid(  row=0, column=1)
	left.grid( row=1, column=0)
	right.grid(row=1, column=2)
	back.grid( row=2, column=1)
	videoButton.grid(row=0, column=0)
	
	root.mainloop()
	
