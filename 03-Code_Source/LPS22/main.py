# main.py -- put your code here!
from machine import I2C
from pyb import delay
import LPS22

# Initialisation du port I2C(2)
i2c = I2C(2)

# Initialisation du capteur LPS22 via l'I2C
lps = LPS22.LPS22(i2c)

# affiche indefiniement la température et la pression toute les demi-secondes
while(True):
    # la fonction get du module lps récupère la température et la pression
    temp, press = lps.get()
    print("temperature: {}, pression: {}".format(temp, press))
    delay(500)

