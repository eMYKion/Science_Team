
#IO lib for Raspberry Pi in Robot Arm, Science Olympiad, December 2015

#Authors: Mayank Mali and Peter Wilson
#Description: Uses hardware-fixed spi interface pins, so no GPIO imports are required

import spidev
import RPi.GPIO as io

#remember to setup SPI PINS
spi = spidev.SpiDev()   #make spi object
spi.open(0,0)    #open spi port 0 on device 0

#io modes
IO_BCM = 0b101
IO_BOARD = 0b011

def IOinit(mode):      #IO_BCM or IO_BOARD
    if(mode==IO_BCM):
        io.setmode(io.BCM)
    elif(mode==IO_BOARD):
        io.setmode(io.BOARD)
def IOquit():
    io.cleanup()

# read SPI data from MCP3008 chip, 8 possible adc's (0 thru 7)
def readadc(adcnum):
    if ((adcnum > 7) or (adcnum < 0)):
        return -1
    r = spi.xfer2([1,(8+adcnum)<<4,0])
    adcout = ((r[1]&3) << 8) + r[2]
    return adcout

def close_readadc(adcnum, val, times=3):
    final = val
    for e in range(0,times):
        temp = readadc(adcnum)
        if(abs(temp-val)<abs(final-val)):
            final = temp
            #now final is closest of the data to original val
    return final


def avg_readadc(adcnum, times):
    total = 0
    for x in range (0, 3):
        discard = readadc(adcnum)
    for x in range (0, times):
        total += readadc(adcnum)
    return total*1.0/times




class IOUnit:
    _inChannelMaster = None
    _inChannelSlave = None
    _outF = None
    _outB = None
    _brake = None
    _pwmF=None
    _dutyF = None
    _pwmB=None
    _dutyB = None
    
    _positionMaster = 0
    _velocityMaster = 0
    _accelerationMaster = 0
    _positionSlave = 0
    _velocitySlave = 0
    _accelerationSlave = 0
    #these are for calculating the velocity + acceleration
    _oldpositionMaster = 0
    _oldvelocityMaster = 0
    _oldpositionSlave = 0
    _oldvelocitySlave = 0

    _PIDval = 0 #this is the value of the PID computed (a duty cycle)
    
    _offset = 0 #potentiometer difference between master and slave: master - slave
    #these are the PID tuning constants
    _k_pos = 0
    _k_vel = 0
    _k_accel = 0

    def dutyFunctionPos(self):
        return self._slope*self._diff+self._intercept
    def dutyFunctionNeg(self):
        return -self._slope*self._diff+self._intercept

    def __init__(self, inChannelMaster=None, inChannelSlave=None, outF=None, outB=None, brake=None, freq=None, dutyF=None, dutyB=None):
        if((inChannelMaster!=None) & (inChannelSlave!=None)):
            self._inChannelMaster = inChannelMaster
            self._inChannelSlave = inChannelSlave
            #spi pins already setup by this point to read
        if(outF!=None):
            self._outF = outF
            io.setup(self._outF, io.OUT)
        if(outB!=None):
            self._outB = outB
            io.setup(self._outB, io.OUT)
        if(brake!=None):
            self._brake = brake
            io.setup(self._brake, io.OUT)
        if((freq!=None) & (dutyF!=None)& (dutyB!=None)):
            self._pwmF = io.PWM(self._outF, freq)
            self._dutyF = dutyF
            self._pwmB = io.PWM(self._outB, freq)
            self._dutyB = dutyB

    #for out
    def outF(self, state):
        io.output(self._outF, state)
    def outB(self, state):
        io.output(self._outB, state)

    #for in
    def inADCMaster(self):
        return readadc(self._inChannelMaster)
    def inADCSlave(self):
        return readadc(self._inChannelSlave)
    def inCloseADCMaster(self, times=3):
        return close_readadc(self._inChannelMaster, self._potValMaster, times)
    def inCloseADCSlave(self, times=3):
        return close_readadc(self._inChannelSlave, self._potValSlave, times)
    def inavgADCMaster(self, times=10):
        return round(avg_readadc(self._inChannelMaster, times), 3)
    def inavgADCSlave(self, times=10):
        return round(avg_readadc(self._inChannelSlave, times), 3)
    def inavgadjustADCSlave(self, times=10):
        return round(avg_readadc(self._inChannelSlave, times) - self._offset, 3)
    def pwmFStart(self):
        self._pwmF.start(0)
    def pwmBStart(self):
        self._pwmB.start(0)
    def pwmFChangeDutyCycle(self, duty):
        self._dutyF = duty
        self._pwmB.ChangeDutyCycle(0)
        self._pwmF.ChangeDutyCycle(duty)
    def pwmBChangeDutyCycle(self, duty):
        self._dutyB = duty
        self._pwmF.ChangeDutyCycle(0)
        self._pwmB.ChangeDutyCycle(duty)
    def brakeon(self):
        self._pwmF.ChangeDutyCycle(0)
        self._pwmB.ChangeDutyCycle(0)
        io.output(self._brake, 1)
    def brakeoff(self):
        io.output(self._brake, 0)
        return 1
    def pwmFStop(self):
        self._pwmF.stop()
    def pwmBStop(self):
        self._pwmB.stop()

    #for PID
    def Pidval_update(self):
        self._PIDval = self._k_pos*(self._positionMaster-self._positionSlave) + self._k_vel*(self._velocityMaster-self._velocitySlave) + self._k_accel*(self._accelerationMaster-self._accelerationSlave)
#end


