#Import des classes delay et Accel du module pyb
#cf. https://docs.micropython.org/en/latest/pyboard/quickref.html

from pyb import delay, Accel

# declaration d'un objet accelerometre
acc = Accel()

while True:
    # acquisition des valeurs sur les 3 axes
    x,y,z = acc.filtered_xyz()
    
    # affichage des valeurs
    print("x:%i y:%i z:%i" %(x,y,z))
    
    delay(200)