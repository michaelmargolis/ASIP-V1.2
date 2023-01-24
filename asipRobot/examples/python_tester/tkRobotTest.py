import asip
import tkinter as tk
import sys
import socket

from tcp_stream import TcpStream

class AsipComs:
    def __init__(self, oneshot, logger, evt_handler):
        self.oneshot = oneshot  # aka root.after
        self.logger = logger
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
            sys.exit()

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
        self.oneshot(50, self.poll)

    def msgDispatcher(self, msg):
        if msg[0] == asip.EVENT_HEADER:
            if msg[1] == asip.SYSTEM_MSG_HEADER:
               info = msg[5:-1].split(',')
               self.logger( 'ASIP version %s.%s running on %s using sketch: %s' % (info[0], info[1],info[2], info[4]))
            else:                    
                self.evt_handler(msg[1], msg[8:-1])
        elif msg[0] == asip.DEBUG_MSG_HEADER:
            self.logger(msg[1:])
        elif msg[0] == asip.ERROR_MSG_HEADER:
            self.logger('Err: ' + msg[1:])          

    def send(self, msg):
        if self.is_connected and self.stream != None:
            if self.stream.send(msg) == False:
                print( "todo - send failed so close port?")
            else:
                return True # successful
        return False        
    
    def setAutoevents(self, svcId, dur):
        self.send(svcId + ',' + asip.tag_AUTOEVENT_REQUEST + ','+ str(dur) + '\n')

    def sendRequest(self, svcId, value):
        print( 'request for svc ' + svcId + ':' + value )
        self.send(svcId + ',' + asip.tag_AUTOEVENT_REQUEST + ','+ value + '\n')


class UI_Utils:
    def __init__(self, root):
        self.root = root

    def create_button(self, parent, text, handler):
        btn = tk.Button(parent, text=text, command=handler, width=8)
        btn.pack(side=tk.LEFT,padx=(10,4))
        return btn
    
    def create_label(self, text):
        lbl = tk.Entry(self.root)
        self.set_text(lbl, text)
        lbl.pack(pady="6", padx="6", fill=tk.X)
        return lbl
      
    def set_text(self, widget, text):
        widget.delete(0,tk.END)
        widget.insert(0,text)
        
class Tester:
    def __init__(self, ui_utils):
        self.ui_utils = ui_utils
        self.coms = None
        self.active_id = None
        self.test_nbr = 0
        self.tests = None
        self.button= None
    
    def start_test(self, id, button, tests, label, caption):
        if self.active_id != None:
            print('another  test is in progress')
            return 
        else:
            self.active_id = id
            self.button = button
            self.test_nbr =0
            self.tests = tests
            self.label = label
            self.caption = caption
            self.button.config(relief=tk.SUNKEN)
            if self.coms:
                self.coms.send('L,W,3,'+'Testing ' + button.cget('text') + '\n' )   
            self.do_test(id)
    
    def do_test(self, id):
        if id == self.active_id:
            if self.test_nbr < len(self.tests):
                print('doing test', self.tests[self.test_nbr])
                if self.label != None:
                    self.ui_utils.set_text(self.label, self.caption + self.tests[self.test_nbr][0] )
                if self.coms:
                    self.coms.send(self.tests[self.test_nbr][1])        
                self.test_nbr += 1
                self.ui_utils.root.after(1000, self.do_test, self.active_id)
            else:
                # done !!
                self.active_id = None
                self.test_nbr=0
                self.tests = None
                self.button.config(relief=tk.RAISED)
                if self.coms:
                    self.coms.send('L,W,3,Test completed\n' )  
    
# test handlers called from button presses

class Tests:
    def __init__(self,  ui_utils, tester):
        # create buttons and labels
        self.ui_utils = ui_utils
        self.tester = tester
        self.frmButtons = tk.Frame(ui_utils.root)                
        self.btnMotors=ui_utils.create_button(self.frmButtons, 'Motors', self.test_motors)
        self.btnSound=ui_utils.create_button(self.frmButtons, 'Sound', self.test_sound)
        self.btnColor=ui_utils.create_button(self.frmButtons, 'Color', self.test_color)
        self.btnServo=ui_utils.create_button(self.frmButtons, 'Servo', self.test_servo)
        self.frmButtons.pack(pady='6')

        self.lbl_motors=ui_utils.create_label("motor info")
        self.lbl_encoders=ui_utils.create_label("encoder info")
        self.lbl_ir=ui_utils.create_label("Ir Sensors")
        self.lbl_bump=ui_utils.create_label("Bump Sensors")
        self.lbl_color=ui_utils.create_label("Color")
        self.lbl_distance=ui_utils.create_label("Distance")
        self.lbl_servo=ui_utils.create_label("Servo")
        self.lb_logger=tk.Listbox(ui_utils.root)
        self.lb_logger.pack(padx='6', pady='6', fill='both')
        
    def test_motors(self):
        tests = (('forward 25%', 'M,M,25,25\n'), ('forward 50%', 'M,M,50,50\n'), ('forward 75%', 'M,M,75,75\n'), ('stopped', 'M,M,0,0\n'),('reverse 50%', 'M,M,-50,-50\n'),('stopped', 'M,M,0,0\n') )
        self.tester.start_test(asip.id_MOTOR_SERVICE, self.btnMotors, tests, self.lbl_motors, "Setting motors to ")

    def test_servo(self):
        tests = ( ('20 degrees','S,W,0,20\n'), ('90 degrees', 'S,W,0,90\n'), ('160 degrees','S,W,0,160\n' ), ('center','S,W,0,90\n'))
        self.tester.start_test(asip.id_SERVO_SERVICE, self.btnServo, tests, self.lbl_servo, "Setting servo to ")
      
    def test_color(self):
        tests = (('Red', 'P,P,0,1,{0:8388608}\n'), ('Green','P,P,0,1,{0:32768}\n'), ('Blue','P,P,0,1,{0:128}\n' ), ('Off','P,P,0,1,{0:0}\n'))
        self.tester.start_test(asip.id_PIXELS_SERVICE, self.btnColor, tests, self.lbl_color, "Setting color to ")

    def test_sound(self):
        tests = (('','T,P,440,800\n'),('','T,P,1000,800\n'),('','T,P,1000,800\n'),('','T,P,440,1000\n'))
        self.tester.start_test(asip.id_TONE_SERVICE, self.btnSound, tests,  None, None)

    def logger(self, msg):
        # print(msg)
        self.lb_logger.insert(0,msg)  
  
    def evtDispatcher(self, id, values):
        fields = values.split(',')
        if id == asip.id_ENCODER_SERVICE:
            self.ui_utils.set_text(self.lbl_encoders,'Encoder values: {}    {}'.format(fields[0], fields[1]))
        elif id == asip.id_BUMP_SERVICE:
            s = ['closed','open  ']
            l = int(fields[0])
            r = int(fields[1])
            self.ui_utils.set_text(self.lbl_bump,'Bump sensors: left {}  right {}'.format(s[l], s[r]))
        elif id == asip.id_IR_REFLECTANCE_SERVICE:
            self.ui_utils.set_text(self.lbl_ir,'Reflectance sensors: left {}  center {}   right {}'.format(fields[0], fields[1], fields[2]))
        elif id == asip.id_DISTANCE_SERVICE:   
             self.ui_utils.set_text(self.lbl_distance,'Distance: {}cm'.format(fields[0]))
             
def main():
    root=tk.Tk()
    root.title("ASIP robot tester")
    root.geometry("350x400")

    if len(sys.argv) > 1:
        addr = sys.argv[1]
        if 'dev' in addr or 'COM' in addr:
            print(sys.argv[1], 'is a com port - not yet implimented')
        else:
            addr = sys.argv[1]
    else:        
        addr = input("enter IP address of robot: ")
    if len(addr) < 7:
        sys.exit()

    
    ui_utils = UI_Utils(root) # user interface helper methods

    tester = Tester(ui_utils) # routine to run preconfigured tests
    tests = Tests(ui_utils, tester)   # the tests to run
    asip_coms = AsipComs(root.after, tests.logger, tests.evtDispatcher) # ASIP TCP connection handler 
    tester.coms = asip_coms
    asip_coms.connect_tcp(addr)
     
    autoevents = [asip.id_BUMP_SERVICE, asip.id_ENCODER_SERVICE, asip.id_IR_REFLECTANCE_SERVICE, asip.id_DISTANCE_SERVICE ]
    for id in autoevents:
        print('Setting autoevent for id', id)
        asip_coms.setAutoevents(id, 100) # all events 100ms
        
    root.after(50, asip_coms.poll)
    root.mainloop()

if __name__ == '__main__':
    main()