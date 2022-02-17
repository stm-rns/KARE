from machine import Pin

class Encoder:
    def __init__(self, pin_x, pin_y, user_cb=None,pin_key=None,key_cb=None):
        self.pin_x = pin_x
        self.pin_y = pin_y
        self.user_cb = user_cb
        self.key_cb = key_cb
        self.state = 0
        self.value = 0
        self.x_Irq = pin_x.irq(trigger=Pin.IRQ_FALLING|Pin.IRQ_RISING, handler=self.x_Callback)
        self.y_Irq = pin_y.irq(trigger=Pin.IRQ_RISING, handler=self.y_Callback)
        
        if pin_key and key_cb:
            self.k_interrupt = pin_key.irq(trigger = Pin.IRQ_FALLING, handler=self.k_callback)       
    
    # Mise à jour du compteur en fin de machine d'état + reset de la machine d'état 
    def cnt_update(self,inc):
        self.state = 0
        self.value += inc
        if self.user_cb: self.user_cb(self.value)    

    def x_Callback(self, line):
        # Démarrage de la machine d'état
        if self.state == 0:
            if self.pin_x.value() == 0:
                # y = 0 => next state = 2
                # y = 1 => next state = 1
                self.state = 2 - self.pin_y.value()
        # Fin de la machine d'état pour le cas incrément (falling edge x puis rising edge y et rising edge x)
        elif self.state == 3:
            if self.pin_x.value() == 1 and self.pin_y.value() == 1:
                self.cnt_update(-1)

    def y_Callback(self, line):
        # Fin de la machine d'état pour le cas décrément (falling edge x puis rising edge x et rising edge y)
        if self.state == 1:
            if self.pin_x.value() == 1 and self.pin_y.value() == 1:
                self.cnt_update(+1)
        # Front montant de y pour le cas incrément
        elif self.state == 2:
            if self.pin_x.value() == 0 and self.pin_y.value() == 1:
                self.state = 3

    def k_callback(self, line):
        if self.key_cb:
            self.key_cb(line)
        
def resetCallBack(p):
    print("0")
    s.value = 0

def printCallBack(p):
    print(s.value)

pin_vpp = Pin('X4',Pin.OUT)
pin_vpp.value(1)
if 1:
    s = Encoder(Pin('X1'),Pin('X2'),printCallBack,  # ==> signaux roue codeuse
                Pin('X3'),resetCallBack)            # ==> signal bouton et sa fonction



