from machine import Pin
from pyb import  Timer

class Encoder:
    def __init__(self, pin_x, pin_y, pin_key=None,key_cb=None):
        self.pin_x = pin_x
        self.pin_y = pin_y
        self.pin_key = pin_key
        self.position = 0
        self.key_cb = key_cb
        self.state = 0
        self.x_level = 0
        self.lastlevel = 1
        
        # Interruption Y: sur front montant et descendant 
        self.y_interrupt = pin_y.irq(trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING, handler=self.y_callback)
        
        # Interruption sur l'appui du bouton 
        if pin_key and key_cb:
            self.k_interrupt = pin_key.irq(trigger = Pin.IRQ_FALLING, handler=self.k_callback)

    def timer_callback(self, timer):
        # On a terminé le filtrage. Arret du timer
        timer.deinit()   
        self.state = 0

        # Si Y est à l'état bas et que le dernier niveau de Y etait à 1
        if self.pin_y.value() == 0:
            if self.lastlevel == 1:
                # L'etat de X quand Y a changé de valeur permet de determiner le sens positif ou négatif 
                self.position += 1 if self.x_level == 0 else -1

            self.lastlevel = 0
        else:
            self.lastlevel = 1
        
    def y_callback(self, line):
        # On vient de recevoir un front descendant ou montant sur le signal Y 
        # Si on n'a pas démarré le timer de filtrage
        if self.state == 0:
           # On mémorise l'état du signal X 
           self.x_level = self.pin_x.value()
           # lancement du timer de 5ms pour filtrer les rebonds
           Timer(4, period=5, callback=self.timer_callback)
           # On indique qu'on a démarré le timer pour filtrer tous les prochains fronts
           self.state = 1
       
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


