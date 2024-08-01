#include <Arduino.h>
#include <ArduinoBLE.h>

// Motor class for simple motor control
class Motor{
    public:
        Motor(int pin1, int pin2);
        void drive(int duty_cycle);
    private:
        int _pin1;
        int _pin2;
        int _pin3;
        int _pin4;
};
// Motor class constructor
Motor::Motor(int pin1, int pin2){
    this->_pin1 = pin1;
    this->_pin2 = pin2;
    pinMode(_pin1, OUTPUT);
    pinMode(_pin2, OUTPUT);
}
// Motor class drive method
void Motor::drive(int duty_cycle){
    if(duty_cycle > 0){
        analogWrite(_pin1, duty_cycle);
        analogWrite(_pin2, 0);
    }
    else if(duty_cycle < 0){
        analogWrite(_pin1, 0);
        analogWrite(_pin2, -duty_cycle);
    }
    else{
        analogWrite(_pin1, 0);
        analogWrite(_pin2, 0);
    }
}