"""
tcp_stream.py

send and receive newline terminated text
"""

import socket
import sys
import threading
from queue import Queue
import traceback

class TcpStream:
    def __init__(self, msg_dispatcher, logger=None, sock=None):
        self.inQ = Queue()
        self.msg_dispatcher = msg_dispatcher
        self.logger = logger
        if sock is None:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        else:
            self.sock = sock
            
    def connect(self, host, port):
        try:
            self.sock.connect((host, port))
            t = threading.Thread(target=self.listener_thread, args=(self.inQ, self.sock))
            t.daemon = True
            t.start()
        except socket.error as exc:
            print("connect caught exception socket.error : %s" % exc)

    def send(self, msg):
        self.sock.sendall(msg.encode())

    def receive(self):
        if not self.inQ.empty():
            msg = self.inQ.get()
            try:
                if msg is not None:
                    msg = msg.rstrip()
                    self.msg_dispatcher(msg)
                    return msg
            except:
                #  print error if input not a string or cannot be converted into valid request
                e = sys.exc_info()[0]
                s = traceback.format_exc()
                print( e, s)
        return None

    def listener_thread(self, inQ, sock):
        MAX_MSG_LEN = 512
        buffer = ''
        while True:
            try:
                data = sock.recv(MAX_MSG_LEN).decode()
                # print( data)
                if data == b'':
                    raise RuntimeError("socket connection broken")
                buffer += data
                while buffer.find('\n') != -1:
                   line, buffer = buffer.split('\n', 1)
                   # print( "got", line)
                   self.inQ.put(line)
            except:
                e = sys.exc_info()[0]
                s = traceback.format_exc()
                print ("listener err", e, s)
"""
def main():
    stream = TcpStream()
    stream.connect("192.168.1.20", 6789)
    while True:
        msg = raw_input("enter msg")
        if len(msg) > 1:
           stream.send(msg)
        elif len(msg) > 0:
            # single char check receive buffer
            s = stream.receive()
            if s != None:
                print (s)
        else:
            exit()
     
     
if __name__ == '__main__':
    main()
"""