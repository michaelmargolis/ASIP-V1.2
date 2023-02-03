/* RobotMotor.cpp
 * Supports motors using Toshiba H-Bridge motor driver ICs.
 * such as Mirto v2 board or Hub-ee 
 * This version has PID speed control
 * Michael Margolis July 2018 added support for drv8833 H-Bridge
 */


#include "RobotMotor.h" 
#include "RobotDescription.h"  // for wheel circumference
#include "utility/asip_debug.h"
      
RobotMotor::RobotMotor(pinArray_t pins[])
{
   this->pins = pins;  
   PID = new MotorPID(ENCODER_TICKS_PER_WHEEL_REV,MAX_PWM, MAX_PWM_DELTA); 
}

RobotMotor::RobotMotor()
{
   PID = new MotorPID(ENCODER_TICKS_PER_WHEEL_REV,MAX_PWM, MAX_PWM_DELTA); 
}

void RobotMotor::begin(const int direction, pinArray_t *pins)
{
   this->pins = pins;  
   begin(direction);
}

void RobotMotor::begin(const int direction)
{
 debug_printf("motor using pins %d,%d,%d\n", pins[0], pins[1], pins[2]) ;  
 // NOTE teensy can use non DRV8833  so H-Bridge type must be set explicitly  
#if defined (UNO_WIFI_REV2_328MODE) || defined (TARGET_RP2040) || defined(TARGET_RASPBERRY_PI_PICO) || defined(ARDUINO_ARCH_ESP32)
  setHbridgeType(_DRV8833); // drv8833 uses pwm so no need to set pinmode  
#endif  
  //pinMode(standbyPin, OUTPUT);
  
  motorDirectionMode = (int)direction ; // flag to set rotation when moving forward 
  targetPwm = currentPwm = 0; 
  motorBrakeMode = false;  // freewheel
  motorStandbyMode = 0;
  stopMotor();
}

void RobotMotor::setHbridgeType(int type)
{
  hBridgeType = type;
  if(type!= _DRV8833){
    // DRV8833 uses analogWrite so no need for pinMode
    pinMode(pins[in1Pin], OUTPUT);
    pinMode(pins[in2Pin], OUTPUT);
  }    
  //Serial.printf("motor using pins %d,%d,%d set to H-bridge type %d\n", pins[0], pins[1], pins[2], type) ; 
}

void RobotMotor::setMotorLabel(const char *labelString) // for debug print only
{
   label = labelString;
   PID->label = label;
}

void RobotMotor::setBrakeMode(boolean brakeMode)
{
  // true shorts motor when stopped, false freewheels
  motorBrakeMode =  brakeMode ? 1:0;
}

void RobotMotor::setDirectionMode(int directionMode)
{
  //set relationship between motor direction and forward movement   
   motorDirectionMode = directionMode;    
   debug_printf("set direction mode to %d\n", directionMode);   
}

#ifdef NOT_NEEDED
int RobotMotor::getDirection()
{
  // range +-MAX_PWM, positive is forward
  return pwm;
}
#endif

inline int RobotMotor::powerToPWM(int power) 
{
  // maps power from range of +-percent to +-MAX_PWM
  power = constrain(power, -100, 100);
  return map(power, -100, 100, -MAX_PWM, MAX_PWM);
}

void RobotMotor::stopMotor()
{
  //stop the motor using the current braking mode 
  PID->stopPid();
  currentPwm = 0;
  setMotorPwm(0);
  debug_printf("%s stopMotor\n", label);
}


void RobotMotor::setStandbyMode(boolean standbyMode)
{
  //set the standby mode if a standby pin has been assigned.
  //invert the value because LOW activates standby on the IC
  if( standbyPin >= 0)
  {
    digitalWrite(standbyPin, motorStandbyMode ? 0:1);
  }
}

void RobotMotor::setMotorPower(int power) // 0-100%
{  
  int pwm = powerToPWM(power);  
  setMotorPwm( pwm ); 
}

void RobotMotor::setMotorPwm(int requestedPwm)
{
  // Serial.print("H bridge type = "); Serial.println(hBridgeType);
  int motorDirection; 
  targetPwm = abs(constrain(requestedPwm, -MAX_PWM, MAX_PWM) );
  if( hBridgeType == _DRV8833){
      // Serial.print("requestedPwm ");  Serial.println(requestedPwm); 
      bool isReverse = requestedPwm < 0;
      bool isInverted = (motorDirection == -1);
      int modeVal = motorBrakeMode? 0 : 255; // value written to non pwm pin dependent on brake mode
      if( isInverted ^ isReverse ^ motorBrakeMode) {
          // here if any one or all three are true
          analogWrite(pins[in2Pin], modeVal);
          //pins[PWMPin] = pins[in1Pin];
          pwmPin = pins[in1Pin];
          //Serial.printf("In %s setPwm, writing %d to pin %d, PWM pin is %d\n", label, modeVal, pins[in2Pin], pwmPin);
      }
      else{
          analogWrite(pins[in1Pin], modeVal);
          //pins[PWMPin] = pins[in2Pin]; 
          pwmPin = pins[in2Pin];
          //Serial.printf("In %s setPwm , writing %d to pin %d, PWM pin is %d\n", label, modeVal, pins[in1Pin], pwmPin);
      }
  }
  else{
    pwmPin = pins[PWMPin];
    if( requestedPwm >= 0)
       motorDirection = NORMAL_DIRECTION;
    else
       motorDirection = REVERSED_DIRECTION;     
    digitalWrite(pins[in1Pin],  !(motorDirection * motorDirectionMode  == 1)? 1:0 );
    //invert the direction for the second control line
    digitalWrite(pins[in2Pin], (motorDirection * motorDirectionMode == 1)? 1:0 );
    //debug_printf("in setMotor: %s pin %d is %d, %d is %d\n", label, pins[in1Pin], digitalRead(pins[in1Pin]),pins[in2Pin], digitalRead(pins[in2Pin])  );
  }
   isRampingPwm();  // control power ramp  
  //debug_printf("in setMotor: %s pwm=%d, targetPwm=%d, dir=%d, dir mode=%d, dir mask=%d\n", label, pwm, targetPwm, motorDirection, motorDirectionMode, (motorDirectionMode ==1));     
}


/*
 * Function to limit motor spin-up acceleration 
 * needed to prevent motor from drawing too much current at startup 
 * isRamping returns true if the current motor pwm is at least as much as the target PWM  
 */
boolean RobotMotor::isRampingPwm() // returns true if motor coming up to speed
{
  if( currentPwm >= targetPwm){
    if(hBridgeType == _DRV8833 && motorBrakeMode == false){ // false is DRV8833 coast mode, so invert the PWM
        analogWrite(pwmPin, 255-targetPwm);
        //debug_printf("in isRampingPwm (inverted), %s writing %d to pin %d but returns false: currentPwm (%d) is >=  targetPwm (%d)\n", label, 255-targetPwm, pwmPin,  currentPwm, targetPwm);
    }
    else{
      analogWrite(pwmPin, targetPwm);
      //debug_printf("in isRampingPwm, %s writing %d to pin %d, but returning false because currentPwm (%d) is >=  targetPwm (%d)\n", label, targetPwm, pwmPin, currentPwm, targetPwm);
   }
    return false;  // motor is getting requested pwm level
  }
#ifndef EXTERNAL_PID_SCHEDULAR  
  if( millis() - prevPwmRampTime >= POWER_RAMP_INTERVAL)  
#endif  
  { 
   // increase power at controlled rate   
   debug_printf("%s PWM ramp from %d ", label, currentPwm);
   currentPwm += MAX_PWM_DELTA;
   if( currentPwm > targetPwm){
         currentPwm = targetPwm;
   }
    if(hBridgeType == _DRV8833 && motorBrakeMode == false){ // false is DRV8833 coast mode, so invert the PWM
        analogWrite(pwmPin, 255-currentPwm);
    }
    else {
       analogWrite(pwmPin, currentPwm);
   }
   debug_printf("to %d\n", currentPwm);
   prevPwmRampTime = millis();
  }   
  return true;
}
   

void RobotMotor::setMotorRPM(int RPM, uint32_t duration) 
{                                                   
  if( abs(RPM) > 0 ) {
      int32_t targetTicksPerSecond = int32_t(((int32_t)RPM * ENCODER_TICKS_PER_WHEEL_REV) / 60 );      
      PID->startPid(targetTicksPerSecond, duration);
      debug_printf("%s %s, ticksPerSecond set to %d, dur=%d\n",
                   label, RPM >= 0 ? "forward":"reversed",targetTicksPerSecond, duration );
  }       
}
