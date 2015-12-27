#example pwm on one elbow motor with a potentiometer

from IOlib import *
import time

PIN_FORWARD = 10
PIN_BACKWARD = 12

PWM_FREQ = 10
DUTY_FORWARD = 0
DUTY_BACKWARD = 0

#we correct these to true values later
IN_CHANNEL_MASTER = 0
IN_CHANNEL_SLAVE = 0


el = IOUnit(IN_CHANNEL_MASTER, IN_CHANNEL_SLAVE, EL_PIN_FORWARD, EL_PIN_BACKWARD, PWM_FREQ, DUTY_FORWARD, DUTY_BACKWARD)
#no input, backward and forward out enabled, backward and forward PWM enabled


IOinit(IO_BCM) #initializes to BCM mode

tolerance = 10

def run_main():
try():
while 1:
el._potValMaster = el.inCloseADCMaster()
el._potValSlave = el.inCloseADCSlave()

if (el._potValMaster - el._potValSlave > tolerance):
	#forward
	el.pwmFChangeDutyCycle(50)
	el.pwmBChangeDutyCycle(0)
elif(el._potValMaster - el._potValSlave < -1*tolerance):
	#backward
	el.pwmBChangeDutyCycle(50)
	el.pwmFChangeDutyCycle(0)
else:
	#still
	el.pwmFChangeDutyCycle(0)
	el.pwmBChangeDutyCycle(0)
except KeyBoardInterruption:
	el.pwmFStop()
	el.pwmBStop()
	IOquit()

run_main()
