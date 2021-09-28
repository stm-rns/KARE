from pyb import LED
from pyb import delay
from random import randint

led_red = LED(1)
led_green = LED(2)
led_orange = LED(3)
led_blue = LED(4)

leds = [led_red,led_green,led_orange,led_blue]

for i in range(1000):
    num = randint(1,4)
    print("num: "+str(num)+"\n")
    leds[num-1].toggle()
    delay(1000)
    
[led.off() for led in leds]
