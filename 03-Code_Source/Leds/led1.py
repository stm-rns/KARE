from pyb import LED
from pyb import delay

led_red = LED(1)
led_green = LED(2)
led_orange = LED(3)
led_blue = LED(4)

leds = [led_red,led_green,led_orange,led_blue]

for led in leds:
    led.on()    
    delay(2000)
    led.off()