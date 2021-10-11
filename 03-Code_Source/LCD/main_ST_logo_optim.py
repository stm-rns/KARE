from random import randint
from machine import SPI
from st7735 import *
import struct

spi = SPI(2, baudrate=60000000, polarity=0, phase=0)
t = TFT(spi, aDC="Y3", aReset="Y4",aCS='Y5')
t.initb2()
t.fill(TFT.FOREST)

def displayBMP24(t,bmp,x0=0,y0=0):
    bmp = open(bmp,'rb')
    data = bmp.read(54)
    bm,file_size,x1,x2,h54,h40,w,h,layers,bits_per_pixel,compression ,x3,res_h,res_v,x7,x8 = struct.unpack("<HLHHLLLLHHLLLLLL", data[:54])
    assert bm == 0x4D42, "bad header 1"
    assert h54 == 54, "bad header 2"
    assert h40 == 40, "bad header 3"
    assert layers == 1, "bad header 4"
    #assert len(data) == file_size, "bad header 5"
    assert bits_per_pixel == 24, "bad header 6"
    assert compression == 0, "bad header 7"

    line_size_in_bytes = bits_per_pixel * w//8
    struct_line = '>%dH' % (w)

    for y in range(h):
        line = bmp.read(line_size_in_bytes) # read bytes from file
        #pixels = [ TFTColor(line[i*3+2],line[i*3+1],line[i*3]) for i in range(w) ] # RGB24 --> RGB565
        pixels = [ ((line[i*3+2] & 0xF8) << 8) | ((line[i*3+1] & 0xFC) << 3) | (line[i*3] >> 3) for i in range(w) ] # RGB24 --> RGB565
        d = struct.pack(struct_line, *pixels) # pack bytearray
        t.image(x0, h-y+y0, w-1+x0, h-y+y0, d) # display
        
displayBMP24(t,'ST.bmp', 0,0)