from machine import Pin
from pyb import  Timer

class Encoder:
    def __init__(self, pin_x, pin_y,pin_key=None,key_cb=None):
        self.forward = True
        self.pin_x = pin_x
        self.pin_y = pin_y
        self.position = 0
        self.key_cb = key_cb
        self.step = 0x11
        self.tim = Timer(4, period=2, callback=self.tim_callback)
       
        if pin_key and key_cb:
            self.k_interrupt = pin_key.irq(trigger = Pin.IRQ_FALLING, handler=self.k_callback)
        
    def tim_callback(self,line):
        x = self.pin_x.value()
        y = self.pin_y.value()
        step = (x << 4)| y
        if step == self.step:
            return
        elif self.step == 0x11: # etat initial
            if step == 0x01: # etat 1 --> forward
                self.forward = True
            elif step == 0x10: # etat 1 --> backward
                self.forward = False
            self.step = step
        elif self.step == 0x10 or self.step == 0x01: # etat 1
            if step == 0x00: # --> etat 2
                self.step = step
            elif step == 0x11: # etat final
                self.step = step
                if self.forward == True:
                    self.position += 1
                else:
                    self.position -= 1
        elif self.step == 0x00: # etat 2
            if step == 0x01 or step == 0x10 :
                self.step = step # --> etat 3
        
    def k_callback(self, line):
        if self.key_cb:
            self.key_cb(line)
            
def myCallBack(p):
    s.position = 0

pin_vpp = Pin('X4',Pin.OUT)
pin_vpp.value(1)
s = Encoder(Pin('X1'),Pin('X2'), # ==> signaux roue codeuse
                Pin('X3'),myCallBack)    # ==> signal bouton et sa fonction    
pos = -1
while True:
    if (s.position != pos):
        pos = s.position
        print(s.position)



