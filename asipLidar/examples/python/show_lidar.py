"""
show_lidar.pyqtgraph

Lidar data received over serial port as comma seperated strings terminated by newline
Data for a scan consists of multiple range messages followed by a range terminator


 Note that range data is sent as over the wire as millimeters and the rotation order of range data is clockwise

  Message format:   
    Range message:
    range_header: "@Nr" 
    scan_id, (value must match value in descriptor message) 
    range_sequence, (must be one more than previous msg for this scan)  
    ranges (mm) (comma seperated distances terminated by newline)

    Range terminator message:
    header: "@Nt",
    scan_id,  (value must match previous range msgs 
    angle_min, (rad)
    angle_max, (rad)
    angle_increment, (rad)
    range_min, (mm)
    range_max, (mm)
    range_len (this must match the number of range values)
    newline terminator
"""
from collections import namedtuple
from PyQt5 import QtCore, QtWidgets
import pyqtgraph as pg
import serial
import numpy as np
import math
import time
import sys
import traceback
import sys
import socket
from tcp_stream import TcpStream

# lidar messages
RangeMsg = namedtuple('RangeMsg', ['header', 'scan_id', 'sequence_id', 'data'])
RangeTermMsg = namedtuple('RangeTermMsg', ['header', 'scan_id',  'angle_min', 'angle_max', 'range_min', 'range_max', 'range_len'])

class Lidar():

    def __init__(self):
         # angles in radians, range values in meters
        self.scan_id = None
        self.sequence_id = None
        self.angle_min = None
        self.angle_max = None
        self.angle_increment = None
        self.range_min = None
        self.range_max = None
        self.range_list = []
        # numpy arrays
        self.ranges = None
        self.angles = None
        self.dbg_perf_time = None
        self.ready_to_plot = False
        
    def process_ranges(self):
        # here following each range termination msg
        self.ranges = np.array(self.range_list)
        self.ranges = self.ranges.astype(np.float32) * 0.001
        # change direction of rotation from cw to ccw
        self.ranges = np.flip(self.ranges)
        self.range_min = np.min(self.ranges)
        self.range_max = np.max(self.ranges)
        self.angles = np.linspace( self.angle_max, self.angle_min, len(self.ranges))
        self.angle_increment = (self.angle_max - self.angle_min) / len(self.ranges)
    
    
    def process_msg(self, raw_msg):
        #return True if scan ready for processing
        m = raw_msg.strip().split(',')
        if raw_msg[2] == 'r': # its a range msg
            try:
                msg_scan_id =  int(m[1])
                sequence_id = int(m[2]) # todo check this
                data = m[3:]
                if self.scan_id != None and self.scan_id != msg_scan_id:
                    print("Error, expected scan id {} got {}".format(self.scan_id, msg_scan_id ))
                    self.scan_id = None # ignore data until after the next term msg 
                else:        
                    self.range_list.extend(data)
            except Exception as e:
                print(e)
                print(traceback.format_exc())
                
        elif raw_msg[2] == 't': # its a term msg
            try:
                msg = RangeTermMsg._make(m)
                # print(msg)
                scan_id = int(msg.scan_id)
                ret = False # default if error in data
                if self.scan_id != None:
                    if self.scan_id != scan_id:
                        print("Ignoring message with mismatched scan id {}->{}".format(scan_id, self.scan_id))        
                    else:                
                        if len(self.range_list) == int(msg.range_len):
                            self.angle_min = float(msg.angle_min) * 0.000174533
                            self.angle_max = float(msg.angle_max) * 0.000174533
                            self.range_min = float(msg.range_min)*.001
                            self.range_max = float(msg.range_max)*.001
                            self.process_ranges()
                            self.ready_to_plot = True
                            ret = True
                        else:
                            print("Expected {} points, got {}".format(msg.range_len, len(self.range_list)))
    
                #prepare for next scan
                self.scan_id = scan_id+1
                self.range_list = []
                if self.dbg_perf_time:
                    pass # print("ms between term msgs =", 1000 * (time.perf_counter() - self.dbg_perf_time)) 
                self.dbg_perf_time =  time.perf_counter()  
                return ret

            except Exception as e:
                print(e)
                print(traceback.format_exc())
        # print("perf =", 1000 * (time.perf_counter() - start))        
        return False                 
 
        
lidar = Lidar()        
    
class MyWidget(pg.GraphicsLayoutWidget):

    def __init__(self, com_port, tcp_addr, parent=None):
        super().__init__(parent=parent)

        try:
            if tcp_addr != '':
                self.tcpComs = asipComs(lidar.process_msg) 
                self.tcpComs.connect_tcp(tcp_addr)
                time.sleep(.5)
                if self.tcpComs.is_connected:
                    self.tcpComs.setAutoevents(1)
                    self.ser = None # disable serial if TCP connects
            elif  com_port != '':
                self.ser = serial.Serial(port=comport, baudrate=115200)
                self.ser.timeout = .1
        except Exception as e:
            print(e)    
            sys.exit()

        self.angles = []
        self.distances = []
        self.timer = QtCore.QTimer(self)
        self.timer.setInterval(2) # in milliseconds
        self.timer.timeout.connect(self.data_update)
        self.timer.start()
    
        self.plotItem = self.addPlot(title="Lidar points")
        self.plotItem.hideAxis('bottom')
        self.plotItem.hideAxis('left')
        self.plotDataItem = self.plotItem.plot([], pen=None, 
            symbolBrush=(255,0,0), symbolSize=5, symbolPen=None)
        self.max_distance = 3
        self.set_plot_ranges(self.max_distance )          
                    

    def set_plot_ranges(self, new_max_distance):
        self.max_distance = math.ceil(new_max_distance)
        self.plotItem.setXRange(-self.max_distance, self.max_distance, padding=0)
        self.plotItem.setYRange(-self.max_distance, self.max_distance, padding=0)
        # Add polar grid lines
        self.plotItem.addLine(x=0, pen=0.2)
        self.plotItem.addLine(y=0, pen=0.2)
        for r in range(1, self.max_distance+1, 1):
            circle = QtWidgets.QGraphicsEllipseItem(-r, -r, r*2, r*2)
            circle.setPen(pg.mkPen(0.2))
            self.plotItem.addItem(circle)
        
    def get_data(self): 
        try:
            if self.tcpComs.is_connected:
                self.tcpComs.poll()
                if lidar.ready_to_plot:
                    if lidar.range_max > self.max_distance:                      
                        self.set_plot_ranges(lidar.range_max)
                    self.plot_data()
            if self.ser:
                while self.ser.in_waiting > 10 :
                    buffer = self.ser.read_until().decode()
                    if buffer[0] == '@' and buffer[1] == 'N':
                        return lidar.process_msg(buffer)
                    elif buffer[0] == '!':
                        print(buffer[1:].strip())

        except serial.serialutil.SerialException:
            print("Com port error, reconnect and restart this script")
            sys.exit()
        except Exception as e:
            print(e)
        return False 

    def plot_data(self):
        x_data = np.sin(lidar.angles)*lidar.ranges
        y_data = np.cos(lidar.angles)*lidar.ranges
        self.plotDataItem.setData(x=x_data, y=y_data)  
        self.ready_to_plot = False        

    def plot_dataX(self):     
        angles = np.array(self.angles)
        angles = angles.astype(np.float32)
        radians = angles * 0.001745329252 # note angles sent are degrees *10
        distances = np.array(self.distances)
        distances = distances.astype(np.float32)
        x_data = np.sin(radians)*distances
        y_data = np.cos(radians)*distances
        self.plotDataItem.setData(x=x_data, y=y_data)         
    
    def data_update(self):    
        if self.get_data():
            self.plot_data()

class asipComs:
    # this version is hard coded for lidar
    def __init__(self, evt_handler):
        self.evt_handler = evt_handler
        self.is_connected = False
        self.stream = None       
            
    def connect_tcp(self,address):
        port = 6789 # default ASIP port
        try:
            self.stream = TcpStream(self.msgDispatcher)
            self.stream.connect(address, port)
            self.is_connected = True
        except socket.error as e:
            print( 'Could not connect to server %s' % address )
            # sys.exit()

    def poll(self):
        if self.is_connected:
            try:
                while True:
                    m = self.stream.receive()
                    if m == None:
                        break
            except socket.error as e: 
                print( "Socket err: ", e)
                self.is_connected = False


    def msgDispatcher(self, msg):
        if msg[:2] == '@N':              
           self.evt_handler(msg)
        elif msg[0] == '!':
            print(msg[1:])          

    def send(self, msg):
        if self.is_connected and self.stream != None:
            if self.stream.send(msg) == False:
                print( "todo - send failed so close port?")
            else:
                return True # successful
        return False        
    
    def setAutoevents(self, dur):
        self.send('N,A,' + ','+ str(dur) + '\n')


        
def main():    
    com_port = 'COM22' # 'COM36' # can be changed by passing cmd line argument
    tcp_addr = '192.168.31.53' # 198'

    
    if len(sys.argv) > 1:
        arg = sys.argv[1]
    else:
       arg = input("Enter ip address or com port for robot: ")
    if 'dev' in arg or 'COM' in arg:
        comport = arg          
        tcp_addr = ''            
    else:
        tcp_addr = arg
        com_port = ''
            
    app = QtWidgets.QApplication([])

    pg.setConfigOptions(antialias=False) # True seems to work as well

    win = MyWidget(com_port, tcp_addr)
    win.show()
    win.resize(800,800) 
    win.raise_()
    app.exec_()

if __name__ == "__main__":
    main()