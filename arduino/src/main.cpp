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
bool firstLoop = true;

// Emergency stop
const float BACKLIMIT = -0.12;
const float FRONTLIMIT = 0.9;

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
    currentState = State::emergencyStop;
    //currentState = State::PIDtest;
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
            //Serial.println("st_wait");
            isRunning ? currentState = State::initReverse : currentState;
            break;

        case State::initReverse:
            //Serial.println("st_initrev");
            to_position = robot.init_reverse_position;
            fromStateStopPendulum = false;
            robot.moveReverse(-1, to_position) ? currentState = State::forward : currentState;
            Serial.println(robot.getPosition());
            break;

        case State::forward:
            //Serial.println("st_forward");
            if (!fromStateStopPendulum)
            {
                (robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                to_position = robot.end_position;
                if (robot.moveForward(robot.fast_speed, to_position, robot.drop_position)) {
                    robot.disableMagnet();
                }
                robot.moveForward(robot.fast_speed, to_position, robot.end_position) ? currentState = State::reverse : currentState;
            }
            else
            {
                (robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                fromStateStopPendulum = false;
                robot.enableMagnet();
                to_position = 0;
                robot.moveForward(-robot.fast_speed, to_position) ? currentState = State::wait : currentState;
            }
            break;

        case State::reverse:
            //Serial.println("st_reverse");
            if (!fromStateStopPendulum)
            {
                (robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                to_position = 0;
                robot.moveReverse(-robot.fast_speed, to_position) ? currentState = State::stopPendulum : currentState;
            }
            else
            {
                (robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                fromStateStopPendulum = false;
                robot.disableMagnet();
                to_position = 0;

                robot.moveReverse(-robot.fast_speed, to_position) ? currentState = State::wait : currentState;
            }
            break;

        case State::stopPendulum :
            Serial.println("STOP PENDULUM");
            if (firstLoop)
            {
                Serial.println("FIRST LOOP");
                firstLoop = false;
                (robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                if (currentState != State::emergencyStop) {
                    currentState = State::forward;
                }
            } 
            else
            {
                Serial.println("NOT FIRST LOOP");
                (robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                fromStateStopPendulum = true;
                timerPid.enable();
                robot.enablePID();
                robot.runPID();

                if (timerPid.toc() > robot.time_stop_pendulum)
                {

                    robot.disablePID();
                    timerPid.disable();
                    if (robot.getPosition() < 0)
                    {
                        Serial.println("FIRST CONDITION");
                        currentState = State::forward;
                    }
                    else if (robot.getPosition() > 0)
                    {
                        Serial.println("SECOND CONDITION");
                        currentState = State::reverse;
                    }
                    else
                    {
                        Serial.println("THIRD CONDITION");
                        currentState = State::wait;
                    }
                }
            }
            break;
        case State::emergencyStop:
            //Serial.println("st_emergency");
            robot.disablePID();
            robot.disableMagnet();
            break;
        case State::PIDtest:
            Serial.println("PIDtest");
            robot.enablePID();
            robot.runPID();
            break;
        }
    }
}

void timerCallback() { shouldSend_ = true; }

void serialEvent() { shouldRead_ = true; }