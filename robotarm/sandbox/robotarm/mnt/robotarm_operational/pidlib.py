class PID:
    def __init__(self, P, I, D):
        self._Kp = P
        self._Ki = I
        self._Kd = D

        self._integrator = 0
        self._derivator = 0

        self._error = 0
        self._prevTime =0

        self_setPoint = 0
        self._currPoint = 0
        
    def setPrevTime(self, time):
        self._prevTime = time
        
    def updateSetPoint(self, point):
        self._setPoint = point

    def updateCurrPoint(self, point):
        self._currPoint = point
        
    def calculatePID(self, currTime, pr = False):

        newError = self._setPoint - self._currPoint

        dTime = currTime - self._prevTime

        #P
        Pval = self._Kp * newError

        #I
        self._integrator += newError * dTime

        if(self._integrator > 1000):
            self._integrator = 1000
        elif(self._integrator < -1000):
            self._integrator = -1000

        Ival = self._Ki * self._integrator

        #D
        self._derivator = ((newError - self._error)/ dTime)/5+4/5*self._derivator

        Dval = self._Kd * self._derivator


        self._error = newError
        self._prevTime = currTime

        if(pr):
            print("%8.3f, %8.3f, %8.3f" % (Pval, Ival, Dval))
        
        return Pval + Ival + Dval
    
        
        

        
        

        
    
