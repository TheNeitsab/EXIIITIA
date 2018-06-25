#include "ExiiitiaLibV3.h"

// ====== Statics declarations ======
volatile bool Hand::m_checkCalib = false;

void Hand::callbackCalibCounter(){
    Hand::m_checkCalib = true;
}

// ====== Methods declarations ======

/*
 * Constructing and initializing Hand object, managing the behavior of the physical hand.
 *
 * @arg sensorData  : structure containing all the informations necessary related to
 *                    the sensor
 * @arg ledData     : structure containing all the informations necessary related to
 *                    the LED
 * @arg indexData   : structure containing all the informations necessary related to
 *                    the index finger
 * @arg thumbData   : structure containing all the informations necessary related to
 *                    the thumb finger
 * @arg otherData   : structure containing all the informations necessary related to
 *                    the other finger
 *
 * @return: NONE
 */
Hand::Hand(SensorData sensorData, LedData ledData, FingerData indexData, FingerData thumbData, FingerData otherData)
{
    m_led = LedRGB(ledData);
    m_indexServo = Servomotor(indexData);
    m_thumbServo = Servomotor(thumbData);
    m_otherServo = Servomotor(otherData);
    m_counter.setCounter(calibCounterNumber, calibTime, callbackCalibCounter);
    m_sensor = Sensor(sensorData, &m_counter);

    m_indexServo.moveAndWait(m_indexServo.getOpenPos());
    m_thumbServo.moveAndWait(m_thumbServo.getOpenPos());
    m_otherServo.moveAndWait(m_otherServo.getOpenPos());
}

/*
 * Launching hand's calibration in order to calibrate sensor's borders.
 *
 * @arg : NONE
 *
 * @return: NONE
 */
void Hand::calibration(void)
{
    m_sensor.calibrate(m_led);
}

/*
 * Moving thumb finger from OPEN to CLOSE position and vice-versa
 * depending on thumb's current state since the it's move is
 * triggered using a pushbutton.
 *
 * @arg : NONE
 *
 * @return: NONE
 */
void Hand::moveThumb(void)
{
    m_thumbServo.changeState();

    if(m_thumbServo.getState()){
        m_thumbServo.move(m_thumbServo.getOpenPos());
    }
    else {
        m_thumbServo.move(m_thumbServo.getClosePos());
    }
}

/*
 * Moving other fingers according to sensor's value.
 *
 * @arg : NONE
 *
 * @return: NONE
 */
void Hand::moveOther(void)
{
    if(m_otherServo.getState()){
        m_otherServo.move(m_sensor);
    }
}

/*
 * Moving index finger according to sensor's value and
 * lighting the LED to highlight the strength transfered
 * to the sensor.
 *
 * @arg : NONE
 *
 * @return: NONE
 */
void Hand::moveIndex(void)
{
    m_indexServo.move(m_sensor);
    m_sensor.lightStrentgh(m_led);
}

/*
 * Updating sensor's value for hand's use.
 *
 * @arg : NONE
 *
 * @return: NONE
 */
void Hand::updateSensorValue(void)
{
    m_sensor.readValue();
}

/*
 * Starting a shifumi game leading to a random hand position
 * between the well known PAPER, ROCK and SCISSORS.
 *
 * @arg : NONE
 *
 * @return: NONE
 */
void Hand::shifumi(void)
{
    m_led.lightLED(PURPLE);
    DEBUG_PRINT("","Sarting SHIFUMI !!");

    int shifumi = random(1,4); //getting a random number between 1 and 3

    switch(shifumi){
        case 1 : //corresponding to ROCK position
          DEBUG_PRINT("","ROCK");
          m_indexServo.move(m_indexServo.getClosePos());
          m_thumbServo.move(m_thumbServo.getClosePos());
          m_otherServo.move(m_otherServo.getClosePos());
          break;
        case 2 : //corresponding to PAPER position
          DEBUG_PRINT("","PAPER");
          m_indexServo.move(m_indexServo.getOpenPos());
          m_thumbServo.move(m_thumbServo.getOpenPos());
          m_otherServo.move(m_otherServo.getOpenPos());
          break;
        case 3 : //corresponding to SCISSORS position
          DEBUG_PRINT("","SCISSORS");
          m_indexServo.move(m_indexServo.getOpenPos());
          m_thumbServo.move(m_thumbServo.getClosePos());
          m_otherServo.move(m_otherServo.getClosePos());
          break;
    }

    int count = 0; //troubleshooting counter related to the pushbutton
    delay(1000);   //slight delay to allow the hand to reach its position

    //blocking loop waiting for user to push on  the button again
    while (1){
        //detaching servo's if they are not active
        if(!m_indexServo.isActive() && !m_thumbServo.isActive() && !m_otherServo.isActive()){
            m_indexServo.sleep();
            m_thumbServo.sleep();
            m_otherServo.sleep();
        }
        //pushbutton's use troubleshooting
        if(digitalRead(7) == LOW){
           count += 1;
        }
        else count = 0;
        if(count == 10) {
          count    =   0;
          DEBUG_PRINT("","End of the game");
          break;
        }
    }
}

/*
 * Detaching all the servos to enhance their lifetime
 *
 * @arg : NONE
 *
 * @return: NONE
 */
void Hand::saveServos(void)
{
    if(!m_indexServo.isActive()){
        m_indexServo.sleep();
    }
    if(!m_thumbServo.isActive()){
        m_thumbServo.sleep();
    }
    if(!m_otherServo.isActive() && (m_sensor.getCurrentValue() == m_sensor.getMinValue())){
        m_otherServo.sleep();
    }
}

/*
 * Constructing and initializing Sensor object, managing the behavior of the sensor.
 *
 * @arg sensorData  : structure containing all the informations necessary related to
 *                    the sensor
 * @arg counter     : pointer on an external homemade counter used to time the
 *                    calibration
 *
 * @return: NONE
 */
Sensor::Sensor(SensorData sensorData, ExtiaCounter *counter):
    m_sensorPin(sensorData.sensorPin),m_currentSensorValue(0),m_minSensor(sensorData.sensorMin),m_maxSensor(sensorData.sensorMax),m_counter(counter)
{

}

/*
 * Calibrating the sensor and lighting the LED according to the strength put on it
 *
 * @arg led : led object from the hand
 *
 * @return: NONE
 */
void Sensor::calibrate(LedRGB led)
{
    DEBUG_PRINT("", "CALIBRATION STARTS !");

    //inverting borders to correctly reset the values
    m_minSensor = 1024;
    m_maxSensor = 0;

    //resetting counter's state
    Hand::m_checkCalib = false;
    //starting counter related to calibration
    m_counter->startCounter(0);

    //starting the calibration process
    while(!(Hand::m_checkCalib)) {
        readValue();        //getting sensor's current value
        lightStrentgh(led); //lighting led accordingly

        //checking borders
        if(m_currentSensorValue < m_minSensor){
            if(m_currentSensorValue >= 0) {
                m_minSensor = m_currentSensorValue;
            }
        }
        else if(m_currentSensorValue > m_maxSensor){
            if(m_currentSensorValue <= 1024) {
                m_maxSensor = m_currentSensorValue;
            }
        }
        DEBUG_PRINT("MIN = ", m_minSensor);
        DEBUG_PRINT("MAX = ", m_maxSensor);
        DEBUG_PRINT("SENSOR = ", m_currentSensorValue);
        DEBUG_PRINT("","============");
    }
    DEBUG_PRINT("", "CALIBRATION HAS ENDED !");
}

/*
 * Lighting the LED according to sensor's current value
 * The LED goes from RED underlining a low strength to
 * GREEN corresponding to a correct amount of force
 *
 * @arg led : led object from the hand
 *
 * @return : NONE
 */
void Sensor::lightStrentgh(LedRGB led)
{
    int R, G, B = 0; //RGB components for LED

    //mapping colors
    R = map(m_currentSensorValue, m_minSensor, m_maxSensor, 255, 0);
    G = map(m_currentSensorValue, m_minSensor, m_maxSensor, 0, 255);

    led.lightLED(R, G, B); //lighting LED
}

/*
 * Getting the value read from the sensor using an average
 * of 10 values in order to smooth the movement and avoid
 * servos flickering
 *
 * @arg : NONE
 *
 * @return : NONE
 */
void Sensor::readValue(void)
{
    int val = 0;

    for(int i = 0; i < 10; i++){
        val += analogRead(m_sensorPin);
    }

    m_currentSensorValue = val/10;
}

/*
 * Getter on current sensors' value
 *
 * @arg : NONE
 *
 * @return : m_currentSensorValue
 */
int Sensor::getCurrentValue(void)
{
    return m_currentSensorValue;
}

/*
 * Getter on minimum sensors' value
 *
 * @arg : NONE
 *
 * @return : m_minSensor
 */
int Sensor::getMinValue(void)
{
    return m_minSensor;
}

/*
 * Getter on maximum sensors' value
 *
 * @arg : NONE
 *
 * @return : m_maxSensor
 */
int Sensor::getMaxValue(void)
{
    return m_maxSensor;
}

/*
 * Constructing and initializing LedRGB object, managing the behavior of the LED.
 *
 * @arg ledData  : structure containing all the informations necessary related to
 *                 the LED
 *
 * @return: NONE
 */
LedRGB::LedRGB(LedData ledData)
{
    m_pinR = ledData.redPin;
    m_pinG = ledData.greenPin;
    m_pinB = ledData.bluePin;
}

/*
 * Lighting the LED according to the color passed in parameter
 *
 * @arg color  : enum of colors available
 *
 * @return: NONE
 */
void LedRGB::lightLED(LedColor color)
{
    switch(color){
    case RED:
        analogWrite(m_pinR, 255);
        analogWrite(m_pinG, 0);
        analogWrite(m_pinB, 0);
        break;

    case GREEN:
        analogWrite(m_pinR, 0);
        analogWrite(m_pinG, 255);
        analogWrite(m_pinB, 0);
        break;

    case BLUE:
        analogWrite(m_pinR, 0);
        analogWrite(m_pinG, 0);
        analogWrite(m_pinB, 255);
        break;

    case PURPLE:
        analogWrite(m_pinR, 255);
        analogWrite(m_pinG, 255);
        analogWrite(m_pinB, 255);
        break;

    case NONE:
        analogWrite(m_pinR, 0);
        analogWrite(m_pinG, 0);
        analogWrite(m_pinB, 0);
        break;

    default:
        analogWrite(m_pinR, 0);
        analogWrite(m_pinG, 0);
        analogWrite(m_pinB, 0);
        break;
    }
}

/*
 * Lighting the LED according to the RGB components passed in parameter
 *
 * @arg R  : red component of the LED
 * @arg G  : green component of the LED
 * @arg B  : blue component of the LED
 *
 * @return: NONE
 */
void LedRGB::lightLED(int R, int G, int B){
    analogWrite(m_pinR, R);
    analogWrite(m_pinG, G);
    analogWrite(m_pinB, B);
}

/*
 * Constructing and initializing Servomotor object, managing the behavior of the servos.
 *
 * @arg fingerData  : structure containing all the informations necessary related to
 *                    the finger's servo
 *
 * @return: NONE
 */
Servomotor::Servomotor(FingerData fingerData):
    m_pin(fingerData.fingerPin),m_closePosValue(fingerData.fingerClose),m_openPosValue(fingerData.fingerOpen),m_speedMin(fingerData.speedMin),m_speedMax(fingerData.speedMax),m_state(true)
{
    m_servo.attach(m_pin);
    move(m_openPosValue); //moving to open position when initialized
}

/*
 * Moving the servo to the position passed in parameter.
 *
 * @arg value  : position's value to go to
 *
 * @return: NONE
 */
void Servomotor::move(int value)
{
    //mapping position and speed according to servo's settings
    int position = map(value, 0, 180, m_openPosValue, m_closePosValue);
    int speed = map(position, m_openPosValue, m_closePosValue, m_speedMin, m_speedMax);
    //checking if servo is attached
    if(!m_servo.attached()){
        wake();
    }
    //going to position
    m_servo.write(position, speed);
}

/*
 * Moving the servo proportionally to sensor's current value.
 *
 * @arg sensor : sensor object
 *
 * @return: NONE
 */
void Servomotor::move(Sensor sensor)
{
    //mapping position and speed according to servo's and sensor's settings
    int position = map(sensor.getCurrentValue(), sensor.getMinValue(), sensor.getMaxValue(), m_openPosValue, m_closePosValue);
    int speed = map(position, m_openPosValue, m_closePosValue, m_speedMin, m_speedMax);

    //handling servos' eventual detachment
    if(position > m_openPosValue){
        if(!m_servo.attached()){
            wake();
        }
        m_servo.write(position, speed);
    }
    else{
        if(m_servo.attached()){
            if(!m_servo.isMoving()){
                sleep();
            }
        }
    }
}

/*
 * Moving the servo to the position passed in parameter and waiting
 * till it finishes its move.
 *
 * @arg value : position's value to go to
 *
 * @return: NONE
 */
void Servomotor::moveAndWait(int value)
{
    //mapping position and speed according to servo's settings
    int position = map(value, 0, 180, m_openPosValue, m_closePosValue);
    int speed = map(position, m_openPosValue, m_closePosValue, m_speedMin, m_speedMax);
    //checking if servo is attached
    if(!m_servo.attached()){
        wake();
    }
    //going to position
    m_servo.write(position, speed, true);
    //detaching servo
    sleep();
}

/*
 * Changing servo's state
 *
 * @arg : NONE
 *
 * @return: NONE
 */
void Servomotor::changeState(void)
{
    m_state = !m_state;
}

/*
 * Getter on servo's state
 *
 * @arg : NONE
 *
 * @return: m_state
 */
bool Servomotor::getState(void)
{
    return m_state;
}

/*
 * Getter on servo's closed position
 *
 * @arg : NONE
 *
 * @return: m_closePosValue
 */
int Servomotor::getClosePos(void)
{
    return m_closePosValue;
}

/*
 * Getter on servo's opened position
 *
 * @arg : NONE
 *
 * @return: m_openPosValue
 */
int Servomotor::getOpenPos(void)
{
    return m_openPosValue;
}

/*
 * Detaching servo
 *
 * @arg : NONE
 *
 * @return: NONE
 */
void Servomotor::sleep(void)
{
    m_servo.detach();
}

/*
 * Attaching servo
 *
 * @arg : NONE
 *
 * @return: NONE
 */
void Servomotor::wake(void)
{
    m_servo.attach(m_pin);
}

/*
 * Checking whether the servo is moving or not
 *
 * @arg : NONE
 *
 * @return: NONE
 */
bool Servomotor::isActive(void)
{
    return m_servo.isMoving();
}
