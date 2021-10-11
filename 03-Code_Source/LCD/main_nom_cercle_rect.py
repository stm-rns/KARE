from ST7735 import *
from machine import Pin, SPI,I2C
from font import terminalfont
from random import randint
if 1:
    spi = SPI(2, baudrate=60000000, polarity=0, phase=0)
    t = TFT(spi, aDC="Y3", aReset="Y4",aCS='Y5')
    t.initb2()
    t.fill(TFT.FOREST)
    t.setvscroll(2,2)
    t.text((0,60),'ST Micro',TFT.BLUE,terminalfont,2)
    t.fillrect((25,10),(60,28),TFT.CYAN)
    t.circle((90,100), 8, TFT.GREEN)
#    while True:
#       for i in range(0,90,1):
#          t.vscroll(i)
#          time.sleep(0.015)
#       for i in range(90,0,-1):
#            t.vscroll(i)
#            time.sleep(0.015)




