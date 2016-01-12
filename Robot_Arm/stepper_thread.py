from lib.stepperlib import *
import sys

COIL_A1 = 5
COIL_A2 = 6
COIL_B1 = 26
COIL_B2 = 13

Init()

stepper1 = stepper(COIL_A1, COIL_A2, COIL_B1, COIL_B2)

steps = sys.argv[1]

thread = Thread(target=stepper1.progress, args=(steps,))
thread.start()
thread.join()



Quit()

