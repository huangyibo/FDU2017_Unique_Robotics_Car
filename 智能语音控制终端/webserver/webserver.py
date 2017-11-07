#!/usr/bin/env python
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer

import rospy
from geometry_msgs.msg import Twist
rospy.init_node('webserver', anonymous=True)
publisher = rospy.Publisher('car_cmd', Twist, queue_size=1)

DEFAULT_PORT=3000

class MYS(BaseHTTPRequestHandler):
    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        self._set_headers()
        self.wfile.write(open('index.html', 'rb').read())

    def do_HEAD(self):
        self._set_headers()

    def do_POST(self):
        self._set_headers()
        length = self.headers.getheaders('content-length')
        data = self.rfile.read(int(length[0]))
        print('Received post data: %s (1:up 2:left 3:right 4:down 5:stop)' % data)
        if len(data)>6:
           data = data.split('=')[1]
        if data == 'up':
            data = 1
        elif data == 'left':
            data = 2
        elif data == 'right':
            data = 3
        elif data == 'down':
            data = 4
        elif data == 'stop':
            data = 5
        else:
            pass
        # publish
        try:
            cmd = Twist()
            cmd.linear.x  = 2
            cmd.linear.y  = int(data)
            cmd.linear.z  = 0.0
            cmd.angular.x = 0.0
            cmd.angular.y = 0.0
            cmd.angular.z = 0.0
            publisher.publish(cmd)
            rospy.loginfo(data)
            print("publish success!\n")
        #except rospy.ROSInterruptException:
        except Exception as e:
            print("publish failed!\n")
            print(e)

def run(server_class=HTTPServer, handler_class=MYS, port=int(DEFAULT_PORT)):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print('Starting httpd...')
    httpd.serve_forever()

if __name__ == "__main__":
    from sys import argv
    try:
        if len(argv) == 2:
            run(port=int(argv[1]))
        else:
            run()
    except Exception as e:
        print(e)
