from threading import *
import time
import RPi.GPIO as io


def Init():
    io.setmode(io.BCM)

def Quit():
    io.cleanup()


class stepper():
    
    _coil_A1 = None
    _coil_A2 = None
    _coil_B1 = None
    _coil_B2 = None

    _virtPos = None
    _delay = None

    #_delayMax = 0.02
    #_delayMin = 0.005
    #_K = 1.2

    _sequence = [[1, 0, 1, 0],[0, 1, 1, 0],[0, 1, 0, 1],[1, 0, 0, 1]]
    
    def __init__(self, A1, A2, B1, B2, delay):
        
        self._coil_A1 = A1
        self._coil_A2 = A2
        self._coil_B1 = B1
        self._coil_B2 = B2

        self._virtPos = 0

        io.setup(self._coil_A1, io.OUT)
        io.setup(self._coil_A2, io.OUT)
        io.setup(self._coil_B1, io.OUT)
        io.setup(self._coil_B2, io.OUT)

        
        self._delay = delay
        
    def delaySec(self):
        time.sleep(self._delay)
        
    def setPins(self, A1, A2, B1, B2):
        io.output(self._coil_A1, A1)
        io.output(self._coil_A2, A2)
        io.output(self._coil_B1, B1)
        io.output(self._coil_B2, B2)

    def setPinsByOne(self, direc):
        temp = self._sequence[(self._virtPos+1)%4]
        self.setPins(temp[0], temp[1], temp[2], temp[3])
        self._virtPos += direc
            
        
        
    #has time.sleep()!!!
    def progress(self, steps):
        if(steps==0):
            self.setPins(0,0,0,0)
        else:
            for x in range(0, abs(steps)):
                self.setPinsByOne(steps/abs(steps))
                #delay = self._delayMin + (self._delayMax-self._delayMin)*(self._K**-x+self._K**(x+1-abs(steps)))
                self.delaySec()
            

