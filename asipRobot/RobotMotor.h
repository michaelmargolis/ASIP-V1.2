/* RobotMotor.h
 * Supports motors using the Toshiba H-Bridge motor driver.
 * such as Mirto v2 board or Hub-ee 
 * Michael Margolis July 2016
 */

#ifndef RobotMotor_h
#define RobotMotor_h
#include "Arduino.h"
#include "MotorPid.h"
#include "Encoder.h"

typedef byte pinArray_t;

enum motorPinIndex {in1Pin,in2Pin,PWMPin,EncApin,EncBpin};

const int NORMAL_DIRECTION    = 1;
const int REVERSED_DIRECTION  = -1;
const int DIR_FORWARD         = 1;
const int DIR_REVERSE         = -1;


const int  MAX_PWM = 255; // maximum PWM value supported by hardware
const int  MAX_PWM_DELTA     = 80;  // max percent increase in power between intervals 
const int  POWER_RAMP_INTERVAL = 30;  // interval between incriments in ms, not used if ASIP controls frame rate
  
enum hBridgeType {_TB6612FNG, _DRV8833};
enum boardType {_UnknownBoard, _Mirto2016Board, _Mirto2018Board, _MirtoUnoWifiBoard};
  
class RobotMotor
{
    public:      
        RobotMotor();  // constructor without pins used for auto board detect
        RobotMotor(pinArray_t pins[], Encoder *encoder);
        //RobotMotor(int In1Pin, int In2Pin, int PWMPin, int STBYPin);
        void begin(const int direction);  
        void begin(const int direction, pinArray_t *pins, Encoder *encoder ); // used for auto board detect
        void setHbridgeType(int type);
        void setBrakeMode(boolean brakeMode);
        void stopMotor();
        void setStandbyMode(boolean standbyMode);
        void setDirectionMode(int mode);  // 1 normal dir, -1 dir inverted
        //int getDirection(); // range +-MAX_PWM, positive is forward
        void setMotorPower(int MPower); // range is -100 to 100                
        void setMotorRPM(int RPM, unsigned long duration); // duration in ms 
        void setMotorPwm(int pwm);        
        void setMotorLabel(const char *label); // for debug print only  
        boolean isRampingPwm(); // returns true if motor coming up to speed        

        // encoder methods
        long encoderDelta();
        long encoderPos();
        void encoderResetCume();

        MotorPID *PID; // todo - make private?
        const char *label; // only used for debug print to identify motor    ;
        
    private:
        void initialise();
        int powerToPWM(int power);
        int targetPwm;  // the absolute value of the requested PWM
        int currentPwm; // most recent PWM driven by ramping acceleration limiter
        int motorDirectionMode;  //1 normal dir, -1 dir inverted
        int motorBrakeMode;  // 0 off, 1 on
        boolean motorStandbyMode;
        //pin assignments: In1, In2, PWM, encoderA, encoderB
        byte *pins;
        byte pwmPin;
        int standbyPin;
        unsigned long prevPwmRampTime;           // time of most increse in PWM to control rate motor comes up to speed      
        Encoder *encoder;
        long prevPos; // previous encoder reading 
        int hBridgeType;

};
#endif


