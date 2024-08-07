/*
 * GRO 302 - Conception d'un robot mobile
 * Code de démarrage
 * Auteurs: Jean-Samuel Lauzon
 * date: 1 mai 2019
 */

/*------------------------------ Librairies ---------------------------------*/
#include <LibS3GRO.h>
#include <ArduinoJson.h>
#include "swing_robot.hpp"
#include "config.hpp"
#include "states_modes.hpp"
#include "timer.hpp"

/*---------------------------- variables globales ---------------------------*/

SwingRobot robot;

// Timer pour json
SoftTimer timerSendMsg_;
bool shouldRead_ = false;
bool shouldSend_ = false;
void timerCallback();
void serialEvent();

// State machine
State currentState = State::wait;
bool isRunning = false;
bool fromStateStopPendulum = false;
bool firstLoop = false;

// Position / vitesses
const float DROP_POSITION = 0.6;
const float END_POSITION = 0.8;
const float SLOW_SPEED = 0.1;
const float FAST_SPEED = 0.7;

Timer timerPid;


void setup()
{
    Serial.begin(BAUD);
    delay(500);
    robot.init();

    pinMode(MAGNET_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    timerSendMsg_.setDelay(UPDATE_PERIODE);
    timerSendMsg_.setCallback(timerCallback);
    timerSendMsg_.enable();
}

void loop()
{
    double to_position = 0;

    if (digitalRead(LEFT_BUTTON))
    {
        if (isRunning)
            isRunning = false;
        else
            isRunning = true;
        delay(500);
    }

    if (shouldRead_)
    {
        robot.readJSON(shouldRead_);
    }

    if (!MANETTE)
    {
        switch (currentState)
        {
        case State::wait:
            robot.enableMagnet();
            Serial.println("st_wait");
            isRunning ? currentState = State::initReverse : currentState;
            break;

        case State::initReverse:
            Serial.println("st_initrev");
            to_position = -0.1;
            fromStateStopPendulum = false;
            robot.moveReverse(-1, to_position) ? currentState = State::forward : currentState;
            Serial.println(robot.getPosition());
            break;

        case State::forward:
            Serial.println("st_forward");
            if (!fromStateStopPendulum)
            {
                to_position = 0.8;
                robot.moveForward(FAST_SPEED, to_position, DROP_POSITION) ? currentState = State::reverse : currentState;
            }
            else
            {
                fromStateStopPendulum = false;
                robot.enableMagnet();
                to_position = 0;
                robot.moveForward(-SLOW_SPEED, to_position) ? currentState = State::wait : currentState;
            }
            break;

        case State::reverse:
            Serial.println("st_reverse");
            if (!fromStateStopPendulum)
            {
                to_position = 0;
                robot.moveReverse(-FAST_SPEED, to_position) ? currentState = State::stopPendulum : currentState;
            }
            else
            {
                fromStateStopPendulum = false;
                robot.disableMagnet();
                to_position = 0;

                robot.moveReverse(-SLOW_SPEED, to_position) ? currentState = State::wait : currentState;
            }
            break;

        case State::stopPendulum :
            Serial.println("st_stoppendulum");
            if (!firstLoop)
            {
                fromStateStopPendulum = true;
                firstLoop = true;
                robot.enablePID();
            }
            if ((timerPid.toc() > 3) && robot.getPosition() < 0)
            {
                robot.disablePID();
                //timerPid = 0;
                currentState = State::forward;
            }
            else if ((timerPid.toc() > 3) && robot.getPosition() > 0)
            {
                robot.disablePID();
                //timerPid = 0;
                currentState = State::reverse;
            }
            else if ((timerPid.toc() > 3))
            {
                robot.disablePID();
                //timerPid = 0;
                currentState = State::wait;
            }
            break;
        }
    }
}

void timerCallback() { shouldSend_ = true; }

void serialEvent() { shouldRead_ = true; }