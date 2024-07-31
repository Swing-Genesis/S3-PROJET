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
bool checkMR;

/*----- PID -----*/
PID pid_;
Timer timerPID;
double PIDmeasurement();
void PIDcommand(double cmd);
void PIDgoalReached();
float cmdCheck;
bool pidEnabled = false;

SwingRobot robot(pid_);

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

    pid_.setGains(0.20, 0, 0.07);
    // Attache des fonctions de retour
    pid_.setMeasurementFunc(PIDmeasurement);
    pid_.setCommandFunc(PIDcommand);
    // pid_.setAtGoalFunc(PIDgoalReached);
    pid_.setEpsilon(0.001);
    pid_.setPeriod(200);
    pid_.setGoal(0);
    pid_.enable();
}

void loop()
{
    double toPosition = 0;
    String tocString;
    String tspString;

    // currentState = State::PIDtest;
    if (digitalRead(LEFT_BUTTON))
    {
        if (isRunning)
        {
            isRunning = false;
        }
        else
        {
            isRunning = true;
        }

        delay(500);
    }

    if (shouldRead_)
    {
        robot.readJSON(shouldRead_);
    }
    if (shouldSend_)
    {
        robot.sendJSON(shouldSend_);
    }

    if (!MANETTE)
    {
        switch (currentState)
        {
        case State::wait:
            robot.enableMagnet();
            //Serial.println("st_wait");

            if (isRunning)
            {
                currentState = State::initReverse;
            }

            break;

        case State::initReverse:
            toPosition = robot.initReversePosition;
            fromStateStopPendulum = false;

            if (robot.moveReverse(1.0f, toPosition))
            {
                currentState = State::forward;
            }

            break;

        case State::forward:
            //Serial.println("st_forward");
            if (!fromStateStopPendulum)
            {
                //(robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                toPosition = robot.endPosition;
                if (robot.moveForward(robot.fastSpeed, toPosition, robot.dropPosition)) {
                    currentState = State::reverse;
                }
            }
            else
            {
                //(robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                fromStateStopPendulum = false;
                robot.enableMagnet();
                toPosition = 0;
                robot.moveForward(robot.fastSpeed, toPosition) ? currentState = State::wait : currentState;
            }
            break;

        case State::reverse:
            //Serial.println("st_reverse");
            if (!fromStateStopPendulum)
            {
                (robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                toPosition = 0;
                robot.moveReverse(robot.fastSpeed, toPosition) ? currentState = State::stopPendulum : currentState;
            }
            else
            {
                (robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                fromStateStopPendulum = false;
                robot.disableMagnet();
                toPosition = 0;

                robot.moveReverse(robot.fastSpeed, toPosition) ? currentState = State::wait : currentState;
            }
            break;

        case State::stopPendulum:
            //Serial.println("STOP PENDULUM");
            if (firstLoop)
            {
                // Serial.println("FIRST LOOP");
                firstLoop = false;
                (robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                if (currentState != State::emergencyStop)
                {
                    currentState = State::forward;
                }
            }
            else
            {
                // Serial.println("NOT FIRST LOOP");
                (robot.getPosition() > FRONTLIMIT || robot.getPosition() < BACKLIMIT) ? currentState = State::emergencyStop : currentState;
                fromStateStopPendulum = true;

                if (!pidEnabled)
                {
                    timerPID.tic();
                    pidEnabled = true;
                    pid_.enable();
                    delay(100);
                }

                if (pidEnabled)
                {
                    pid_.run();
                }

                // prints timerPID.toc() for debugging
                // tocString = String(timerPID.toc(), 4);
                // tspString = String(robot.time_stop_pendulum, 4);
                // Serial.println("Toc : " + tocString);
                // Serial.println("Time stop pendulum : " + tspString);
                if (timerPID.toc() > robot.timeStopPendulum)
                {

                    pid_.disable();
                    pidEnabled = false;
                    timerPID.reset();
                    if (robot.getPosition() < 0)
                    {
                        // Serial.println("FIRST CONDITION");
                        currentState = State::forward;
                    }
                    else if (robot.getPosition() > 0)
                    {
                        // Serial.println("SECOND CONDITION");
                        currentState = State::reverse;
                    }
                    else
                    {
                        // Serial.println("THIRD CONDITION");
                        currentState = State::wait;
                    }
                }
            }
            break;
        case State::emergencyStop:
            // Serial.println("st_emergency");
            pid_.disable();
            pidEnabled = false;
            robot.setSpeed(0);
            robot.disableMagnet();
            break;
        case State::PIDtest:
            pid_.run();
            break;
        }
    }
    timerSendMsg_.update();
}

// Fonctions pour le PID
double PIDmeasurement()
{

    float analogValue = analogRead(POTENTIOMETER_PIN);
    double pendulumAngle = (Helpers::floatMap(analogValue, 170, 960, -180, 180) + 3);
    return pendulumAngle;
}

// the cmd calculated by compute command is then used with a weight to lower or augment speed
void PIDcommand(double cmd)
{
    cmdCheck = cmd * 0.075;
    robot.setSpeed(cmdCheck * 0.35);
}

void PIDgoalReached(double pendulumAngle)
{
    Timer timer;
    bool timerON = false;
    if (abs(pendulumAngle < 15))
    {
        timer.tic();
        timerON = true;
    }
    while (timerON)
    {
        pid_.run();
        if (abs(pendulumAngle > 15))
        {
            timer.tic();
            break;
        }
        if (timer.toc() > 0.5)
        {
            return; // valeur pour dire d<arreter PID
        }
    }
}

void timerCallback() { shouldSend_ = true; }

void serialEvent() { shouldRead_ = true; }