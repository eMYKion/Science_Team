#shoulder + elbow on/off code, using 4 pots 

<<<<<<< HEAD
from lib.PIDlib import *
=======
from PIDlib import *
>>>>>>> bea14978fc20bf2de6f2105c7c56bf9f4a87c32c
import time



EL_PIN_FORWARD = 21
EL_PIN_BACKWARD = 16
EL_BRAKE_PIN = 19

#replace later
SH_PIN_FORWARD = 17
SH_PIN_BACKWARD = 27
SH_BRAKE_PIN = 20


PWM_FREQ = 1000
DUTY_FORWARD = 100
DUTY_BACKWARD = 100


#we correct these to true values later
EL_IN_CHANNEL_MASTER = 1
EL_IN_CHANNEL_SLAVE = 3
SH_IN_CHANNEL_MASTER = 0
SH_IN_CHANNEL_SLAVE = 2

IOinit(IO_BCM) #initializes to BCM mode

el = IOUnit(EL_IN_CHANNEL_MASTER, EL_IN_CHANNEL_SLAVE, EL_PIN_FORWARD, EL_PIN_BACKWARD, EL_BRAKE_PIN, PWM_FREQ, DUTY_FORWARD, DUTY_BACKWARD)
#no input, backward and forward out enabled, brake enabled, backward and forward PWM enabled

sh = IOUnit(SH_IN_CHANNEL_MASTER, SH_IN_CHANNEL_SLAVE, SH_PIN_FORWARD, SH_PIN_BACKWARD, SH_BRAKE_PIN, PWM_FREQ, DUTY_FORWARD, DUTY_BACKWARD)
#no input, backward and forward out enabled, brake enabled, backward and forward PWM enabled

el.pwmFStart()
el.pwmBStart()
sh.pwmFStart()
sh.pwmBStart()

#set constants
el.setPID(4, 0.5, 0)
sh.setPID(1, 0.2, 0)

sh._offset = round(sh.inavgADCMaster(50) - sh.inavgADCSlave(50), 3)
el._offset = round(el.inavgADCMaster(50) - el.inavgADCSlave(50), 3)
print(el._offset)

el._positionMaster = el.inavgADCMaster()
el._positionSlave = el.inavgadjustADCSlave()
sh._positionMaster = el.inavgADCMaster()
sh._positionSlave = el.inavgadjustADCSlave()

def run_main():
  try:
    oldtime  = time.time()
    while 1:
      
      newtime = time.time()
      dtime = newtime - oldtime
      oldtime = newtime

      
      
      el.Pidval_update(dtime)
      sh.Pidval_update(dtime)
    

      print(el._PIDval, el._positionMaster, el._velocityMaster, el._positionSlave, el._velocitySlave)
      
      
      #need to fix the signs
      if sh._PIDval>0:
        sh.brakeoff()
        if sh._PIDval>100:
          sh.pwmBChangeDutyCycle(100)
        else:
          sh.pwmBChangeDutyCycle(sh._PIDval)
      elif sh._PIDval<0:
        sh.brakeoff()
        if sh._PIDval<-100:
          sh.pwmFChangeDutyCycle(100)
        else:
          sh.pwmFChangeDutyCycle(sh._PIDval)
      else:
        sh.brakeon()
        

      if el._PIDval>0:
        el.brakeoff()
        if el._PIDval>100:
          el.pwmFChangeDutyCycle(100)
        else:
          el.pwmFChangeDutyCycle(el._PIDval)
      elif el._PIDval<0:
        el.brakeoff()
        if el._PIDval<-100:
          el.pwmBChangeDutyCycle(100)
        else:
          el.pwmBChangeDutyCycle(el._PIDval)
      else:
        el.brakeon()
        

      el.setNewToOld()
      sh.setNewToOld()

      
      
  except KeyboardInterrupt:
    el.pwmFStop()
    el.pwmBStop()
    sh.pwmFStop()
    sh.pwmBStop()
    IOquit()

run_main()


