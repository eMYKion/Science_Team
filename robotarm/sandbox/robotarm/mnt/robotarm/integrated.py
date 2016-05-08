from lib.stepperlib import *
from lib.IOlib import *
import sys, time

#shoulder + elbow on/off code, using 4 pots 


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

EL_IN_CHANNEL_MASTER = 1
EL_IN_CHANNEL_SLAVE = 3
SH_IN_CHANNEL_MASTER = 0
SH_IN_CHANNEL_SLAVE = 2
STEPPER_IN_CHANNEL_MASTER = 4



IOinit(IO_BCM)
stepperInit()

COIL_A1 = 5
COIL_A2 = 6
COIL_B1 = 26
COIL_B2 = 13

VELOCITY_MIN = 1
VELOCITY_MAX = 100

steppers = stepper(COIL_A1, COIL_A2, COIL_B1, COIL_B2, 1/VELOCITY_MIN)

stepperThread = None


el = IOUnit(EL_IN_CHANNEL_MASTER, EL_IN_CHANNEL_SLAVE, EL_PIN_FORWARD, EL_PIN_BACKWARD, EL_BRAKE_PIN, PWM_FREQ, DUTY_FORWARD, DUTY_BACKWARD)
sh = IOUnit(SH_IN_CHANNEL_MASTER, SH_IN_CHANNEL_SLAVE, SH_PIN_FORWARD, SH_PIN_BACKWARD, SH_BRAKE_PIN, PWM_FREQ, DUTY_FORWARD, DUTY_BACKWARD)
base = IOUnit(STEPPER_IN_CHANNEL_MASTER)



el._tolerance = 2
el._tol_max = 42
el._slope = 100/(el._tol_max - el._tolerance)
el._intercept = -el._tolerance*el._slope

sh._tolerance = 2
sh._tol_max = 42
sh._slope = 100/(sh._tol_max - sh._tolerance)
sh._intercept = -sh._tolerance*sh._slope

el.pwmFStart()
el.pwmBStart()
sh.pwmFStart()
sh.pwmBStart()


for x in range (0, 10):
  sh._offset += round(sh.inavgADCMaster() - sh.inavgADCSlave(),3)
  el._offset += round(el.inavgADCMaster() - el.inavgADCSlave(),3)
sh._offset = round(sh._offset/10, 3)
el._offset = round(el._offset/10, 3)


velProfiles = []

def runProfiles():
  count=0
  while(1):
    if(len(velProfiles)>count+1):
      count+=1
      increment(velProfiles[count])

stepperThread =Thread(target=runProfiles)

def increment(velocity):
    
        steppers.setPinsByOne(1)
        steppers._delay = 1.0/currentVel
        #print(str(steppers._virtPos)+', '+str(currentVel)+', '+str(steppers._delay))
        time.sleep(steppers._delay)

        
        

def run_main():
  try: 
    while 1:

        base._potValMaster = base.inavgADCMaster()
        error = base._potValMaster - stepper1.virtPos

        currentVel=0
        global velProfiles

            
    
        if(5<error<30):
            
            
          currentVel += (VELOCITY_MAX - currentVel) / (abs(error))
            
        elif(30<=error):
            
          currentVel = VELOCITY_MAX
            
        elif(-30 < error < -5):
          currentVel += (VELOCITY_MAX - currentVel) / (abs(error))

        elif(error <= -30):

          currentVel = VELOCITY_MAX
        else:
          currentVel = 0

        velProfiles.append(currentVel)
        
        
        el._potValMaster = el.inavgADCMaster()
        el._potValSlave = el.inavgADCSlave()
        el._diff = round(el._potValMaster - el._potValSlave - el._offset, 2)
        sh._potValMaster = sh.inavgADCMaster()
        sh._potValSlave = sh.inavgADCSlave()
        sh._diff = round(sh._potValMaster - sh._potValSlave - sh._offset, 2)

      
      
      
        print( "SH_M %4.2f SH_S %4.2f SH_D %4.2f || EL_M %4.2f EL_S %4.2f EL_D %4.2f" % (sh._potValMaster,sh._potValSlave,sh._diff,el._potValMaster,el._potValSlave,el._diff))
        if (el._diff > el._tolerance):
          el.brakeoff()
          #forward
          el.pwmBChangeDutyCycle(0)
          print('EL FORWARD')

          
          if(el._diff>el._tol_max):#max
            el.pwmFChangeDutyCycle(100)
          else:
            el.pwmFChangeDutyCycle(el.dutyFunction())
            
        elif(el._diff < -1*el._tolerance):
          #backward
          print('EL BACKWARD')
          el.brakeoff()
          el.pwmFChangeDutyCycle(0)

          if(el._diff<-1*el._tol_max):
            el.pwmBChangeDutyCycle(100)
          else:
            el.pwmBChangeDutyCycle(el.dutyFunction())
        else:
          #still
          print('EL STILL')
          el.brakeon()



          
        if (sh._diff < -1*sh._tolerance):
          #forward
          print('SH FORWARD')
          sh.brakeoff()
          sh.pwmBChangeDutyCycle(0)

          if(sh._diff<-1*sh._tol_max):
            sh.pwmFChangeDutyCycle(100)
          else:
            sh.pwmFChangeDutyCycle(sh.dutyFunction())
          
        elif(sh._diff > sh._tolerance):
          #backward
          print('SH BACKWARD')
          sh.brakeoff()
          sh.pwmFChangeDutyCycle(0)

          if(sh._diff>sh._tol_max):
            sh.pwmBChangeDutyCycle(100)
          else:
            sh.pwmBChangeDutyCycle(sh.dutyFunction())
          
        else:
          #still
          print('SH STILL')
          sh.brakeon()



  except KeyboardInterrupt:
    el.pwmFStop()
    el.pwmBStop()
    sh.pwmFStop()
    sh.pwmBStop()
    IOquit()
    stepperQuit()
    
stepperThread.start()
run_main()

