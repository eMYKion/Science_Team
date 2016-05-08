//IO lib for Raspberry Pi in Robot Arm, Science Olympiad, December 2015

/*	Authors: Mayank Mali and Peter Wilson
*	Description: Uses hardware-fixed spi interface pins, so no GPIO imports are required
*/

#include "Potentiometer.h"
#include "Spi.h"
#include <wiringPi.h>

Potentiometer::Potentiometer(int ch){
	this->_chPin = ch;
	this->_internalOffset=0;

}

Potentiometer::~Potentiometer(void){

}

int Potentiometer::getValue(void){
	return spiReadADCDifferential(this->_chPin) + this->_internalOffset;
}

void Potentiometer::setValue(double val){
	this->_internalOffset = -this->getValue() + val;
}



/*

import spidev
import RPi.GPIO as io

#remember to setup SPI PINS
spi = spidev.SpiDev()   #make spi object
spi.open(0,0)    #open spi port 0 on device 0
spi.max_speed_hz=(3000000) #set SPI speed

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

# read SPI data from MCP3008 chip, 8 possible channels (0 thru 7)
def readadc(channel):
    adc = spi.xfer2([12+((6&channel)>>1),(1&channel)<<7,0])
    data = ((adc[1]&15) << 8) + adc[2]
    return data

def readadc_diff(channel):
    adc = spi.xfer2([8+((6&channel)>>1),(1&channel)<<7,0])
    data = ((adc[1]&31) << 8) + adc[2]
    return data


def avg_readadc(adcnum, times=5):
    total = 0
    for x in range (0, times):
        total += readadc(adcnum)
    return total*1.0/times

def avg_diff_readadc(adcnum, times=5):
    total = 0
    for x in range (0, times):
        total += readadc_diff(adcnum)
    data = total*1.0/times
    if data > 4096:
        data = data-8192
    return data

def rolling_readadc(adcnum, previous_value, times=5):
    return previous_value/2 + avg_readadc(adcnum, times)/2





class IOUnit:
    _inChannelMaster = None
    _inChannelSlave = None
    _outF = None #GPIO pin
    _outB = None #GPIO pin
    _brake = None #GPIO pin
    _pwmF=None
    _dutyF = None
    _pwmB=None
    _dutyB = None
    _potValMaster = 0
    _potValSlave = 0
    _offset = 0 #initial difference between the slave and the master
    _tolerance = 0 #bound on difference, less than this -> no movement
    _tol_max = 0 #bound on pot difference, less than this -> PWM movement, mofre than this -> full power
    _slope = 0 #for PWM accel/decel function
    _intercept = 0 #for PWM accel/decel function

    def __init__(self, inChannelMaster=None, inChannelSlave=None, outF=None, outB=None, brake=None, freq=None, dutyF=None, dutyB=None):
        if((inChannelMaster!=None) | (inChannelSlave!=None)):
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
    def diff_readadc(self):
        return avg_diff_readadc(self._inChannelMaster)
    def inavgADCMaster(self):
        return avg_readadc(self._inChannelMaster)
    def inavgADCSlave(self):
        return avg_readadc(self._inChannelSlave)
    def rolling_readadc_Master(self, previous_value):
        return rolling_readadc(self._inChannelMaster, previous_value)
    def rolling_readadc_Slave(self, previous_value):
        return rolling_readadc(self._inChannelSlave, previous_value)
    def pwmFStart(self):
        self._pwmF.start(0)
    def pwmBStart(self):
        self._pwmB.start(0)
    def pwmFChangeDutyCycle(self, duty):
        
        self._dutyF = duty
        self._pwmF.ChangeDutyCycle(duty)
        
        return 1
    def pwmBChangeDutyCycle(self, duty):
        
        self._dutyB = duty
        self._pwmB.ChangeDutyCycle(duty)
        
        return 1
    def dutyFunction(self):
        return self._slope*abs(self._diff)+self._intercept
    def brakeon(self):
        
        self._pwmF.ChangeDutyCycle(0)
        self._pwmB.ChangeDutyCycle(0)
        
        io.output(self._brake, 1)
        
        return 1
    def brakeoff(self):
        io.output(self._brake, 0)
        return 1
    def pwmFStop(self):
        self._pwmF.stop()
    def pwmBStop(self):
        self._pwmB.stop()
#end

*/
