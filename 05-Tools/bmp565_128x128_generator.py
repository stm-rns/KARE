import os,sys,struct
from PIL import Image
from tkinter import filedialog
 
def write_bin(f, pixel_list):
    for pix in pixel_list:
        r = (pix[0] >> 3) & 0x1F
        g = (pix[1] >> 2) & 0x3F
        b = (pix[2] >> 3) & 0x1F
        f.write(struct.pack('H', (r << 11) + (g << 5) + b))

path = filedialog.askopenfilename(filetypes=[("jpeg","*.jp*g"),("png","*.png"),("bmp","*.bmp"),("all files", "*")])

if not path:
    sys.exit(1)

im = Image.open(path)
im = im.resize((128,128))
im = im.convert('RGB')
pixels = list(im.getdata())

initialfile = os.path.splitext(path.split(os.sep)[-1])[0]+'.bin'
out_path = filedialog.asksaveasfilename(filetypes=[("bin","*.bin")],initialfile=initialfile)

if not out_path:
    sys.exit(2)

with open(out_path, 'wb') as f:
    write_bin(f, pixels)