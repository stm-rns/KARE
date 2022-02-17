# main.py -- put your code here!
from machine import Pin
from pyb import delay

pin_vpp = Pin('X4',Pin.OUT)
pin_vpp.value(1)
pin_x = Pin('X1')
pin_y = Pin('X2')
pin_key = Pin('X3')

cur_x = last_x = pin_x.value()
cur_y = last_y = pin_y.value()

# Machine à 5 états (0, 1, 2, 3, 4) pour passer sur les 4 fronts descendants/montants de X et Y
automaton = 0

#X  ---        --3-----
#    0 |_1____|

#Y  -------         --4--
#          |_2_____|



#X  -------         --14--
#          |_12____|

#Y  ---         -13-----
#    0 |_11____|
    

# Indication de sens (1, -1) ou 0 : pas démarré
forward = 0
# Position courante
position = 0
    
while True:
    
    cur_x = pin_x.value()
    cur_y = pin_y.value()
    
    if (last_x != cur_x):
        # x a changé
        last_x = cur_x
        if cur_x == 0:
            # Front descendant de x 
            if automaton == 0:
                # On n'avait pas encore démarré: on démarre dans le sens positif 
                forward = 1
                automaton = 1
            elif automaton == 11:
                automaton = 12
        else:
            # Front montant de x 
            if automaton == 2:
                automaton = 3
            elif automaton == 13:
                automaton = 14
            
    if (last_y != cur_y):
        # y a changé
        last_y = cur_y
        if cur_y == 0:
            # Front descendant de y 
            if automaton == 0:
                # On n'avait pas encore démarré: on démarre dans le sens négatif 
                forward = -1
                automaton = 11
            elif automaton == 1:
                automaton = 2
        else:
            # Front montant de y 
            if automaton == 3:
                automaton = 4
            elif automaton == 12:
               automaton = 13
            
    if automaton == 4 or automaton == 14:
        position += forward
        print(position)
        automaton = 0
        forward = 0



