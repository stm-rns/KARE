import os,sys,struct,wx
from PIL import Image

def write_bin(f, pixel_list):
    for pix in pixel_list:
        r = (pix[0] >> 3) & 0x1F
        g = (pix[1] >> 2) & 0x3F
        b = (pix[2] >> 3) & 0x1F
        f.write(struct.pack('H', (r << 11) + (g << 5) + b))

app = wx.App()
dlg = wx.FileDialog(None,"Pick up file",'.',"","JPG|*.jpg|PNG|*.png|BMP|*.bmp|*.*|*.*",wx.FD_OPEN)
if wx.ID_OK !=  dlg.ShowModal():
    wx.MessageBox('canceled','canceled')
    dlg.Destroy()
    sys.exit()

path = dlg.GetPath()
dlg.Destroy()

im = Image.open(path)
im = im.resize((128,128))
im = im.convert('RGB')
pixels = list(im.getdata())

dlg = wx.FileDialog(None,"Pick up file",'.',"output.bin","BIN|*.bin|PNG|*.png|BMP|*.bmp|*.*|*.*",wx.FD_SAVE)
if wx.ID_OK !=  dlg.ShowModal():
    wx.MessageBox('canceled','canceled')
    dlg.Destroy()
    sys.exit()

out_path = dlg.GetPath()
dlg.Destroy()

with open(out_path, 'wb') as f:
    write_bin(f, pixels)