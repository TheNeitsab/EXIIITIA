#ifndef EXIIITIA_LIB_V3_H_
#define EXIIITIA_LIB_V3_H_

// ==========================  INCLUDES  ==========================

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <ExtiaCounter.h>

// =======================  DEBUG SETTINGS  =======================

#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x,y) { Serial.print (x); Serial.println (y); }
#else
 #define DEBUG_PRINT(x,y)
#endif // DEBUG

// ======================  GLOBAL SETTINGS  ========================

//data structure gathering the informations related to fingers' init
struct FingerData {
    int fingerPin;
    int fingerOpen;
    int fingerClose;
    int speedMin;
    int speedMax;
};
//data structure gathering the informations related to sensor's init
struct SensorData {
    int sensorPin;
    int sensorMin;
    int sensorMax;
};
//data structure gathering the informations related to LED' init
struct LedData {
    int redPin;
    int greenPin;
    int bluePin;
};
//enum gathering the key words for lighting the RGB LED
enum LedColor
{
    RED,
    GREEN,
    BLUE,
    PURPLE,
    NONE
};
//index's settings
int const indexPin      =   2;
int const indexMinPos   =   25;
int const indexMaxPos   =   128;
int const indexMinSpeed =   50;
int const indexMaxSpeed =   100;
//thumb's settings
int const thumbPin      =   6;
int const thumbMinPos   =   20;
int const thumbMaxPos   =   148;
int const thumbMinSpeed =   50;
int const thumbMaxSpeed =   80;
//other's settings
int const otherPin      =   5;
int const otherMinPos   =   8;
int const otherMaxPos   =   100;
int const otherMinSpeed =   50;
int const otherMaxSpeed =   100;
//RGB LED's settings
int const redPin    =   12;
int const greenPin  =   13;
int const bluePin   =   9;
//sensor's settings
int const sensorPin =   A0;
int const sensorMin =   0;
int const sensorMax =   1024;
//calibration's part settings
int const calibButtonPin        =   11;
int const calibCounterNumber    =   0;
int const calibTime             =   4000;
//thumb pushbutton's settings
int const thumbButtonPin = 8;
int const shifumiButtonPin = 7;

// =====================  CLASS DEFINITIONS  =======================

class LedRGB {
public :
    LedRGB(){};
    LedRGB(LedData ledData);
    void lightLED(LedColor color);
    void lightLED(int R, int G, int B);
private :
    int m_pinR;     //red pin of the RGB Led
    int m_pinG;     //green pin of the RGB Led
    int m_pinB;     //blue pin of the RGB Led
};

class Sensor {
public :
    Sensor(){};
    Sensor(SensorData sensorData, ExtiaCounter *counter);
    void calibrate(LedRGB led);
    void lightStrentgh(LedRGB led);
    void readValue(void);
    int getCurrentValue(void);
    int getMinValue(void);
    int getMaxValue(void);

private :
    int m_sensorPin;            //sensor's pin
    int m_currentSensorValue;   //current value read from the sensor
    int m_minSensor;            //inferior border of the sensor range
    int m_maxSensor;            //superior border of the sensor range
    ExtiaCounter *m_counter;    //counter pointer for calibration
};

class Servomotor {
public :
    Servomotor(){};
    Servomotor(FingerData fingerData);
    void move(int value);
    void move(Sensor sensor);
    void moveAndWait(int value);
    void changeState(void);
    bool getState(void);
    int getClosePos(void);
    int getOpenPos(void);
    bool isActive(void);
    void sleep(void);
    void wake(void);

private :
    VarSpeedServo m_servo;  //servo's instance of the VarSpeedServo library to handle speed control
    int m_pin;              //servo's pin
    int m_closePosValue;    //servo's close position value
    int m_openPosValue;     //servo's open position value
    int m_speedMin;         //servo's maximum motion speed
    int m_speedMax;         //servo's minimum motion speed
    bool m_state;
};

class Hand {
public :
    Hand(){};
    Hand(SensorData sensorData, LedData ledData, FingerData indexData, FingerData thumbData, FingerData otherData);
    void calibration(void);
    void moveThumb(void);
    void moveOther(void);
    void moveIndex(void);
    void updateSensorValue(void);
    void shifumi(void);
    void saveServos(void);

    volatile static bool m_checkCalib;
    static void callbackCalibCounter(void);

private:
    ExtiaCounter m_counter;     //home made counters for interruptions
    Sensor m_sensor;            //sensor instance part of the hand
    LedRGB m_led;               //led instance part of the hand
    Servomotor m_indexServo;    //index's servo part of the hand
    Servomotor m_thumbServo;    //thumb's servo part of the hand
    Servomotor m_otherServo;    //other's servo part of the hand
};

#endif // EXIIITIA_LIB_V3_H_

