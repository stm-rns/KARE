from ST7735 import *
from machine import Pin, SPI,I2C
from font import terminalfont
from random import randint

spi = SPI(2, baudrate=60000000, polarity=0, phase=0)
t = TFT(spi, aDC="Y3", aReset="Y4",aCS='Y5')
t.initb2()
t.fill(TFT.FOREST)

with open('output.bin', 'rb') as f:
    for y in range(128):
       for x in range(128):
          data = (f.read(2))
          d = (data[1]<<8)|(data[0])
          pos = [x,y]
          t.pixel(pos, d)

