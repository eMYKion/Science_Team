from lib.stepperlib import *
from threading import *
import sys

io.setmode(io.BCM)
COIL_A1 = 5
COIL_A2 = 6
COIL_B1 = 26
COIL_B2 = 13
DELAY = float(sys.argv[2])

stepperInit()

stepper1 = stepper(COIL_A1, COIL_A2, COIL_B1, COIL_B2, DELAY)

steps = int(sys.argv[1])


thread = Thread(target=stepper1.progress, args=(steps,))

try:
    thread.start()
    thread.join()
except KeyboardInterrupt:
    pass



stepperQuit()

