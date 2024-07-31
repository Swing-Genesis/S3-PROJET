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
}

void loop()
{
    double toPosition = 0;
    String tocString;
    String tspString;

    //currentState = State::PIDtest;
    // if (digitalRead(LEFT_BUTTON))
    // {
    //     if (isRunning)
    //     {
    //         currentState = State::wait;
    //         robot.init();
    //         robot.setPosition(0);
    //         firstLoop = true;
    //         isRunning = false;
    //     }
    //     else
    //     {
    //         isRunning = true;
    //     }

    //     delay(500);
    // }

    if(robot.autoActivated && !isRunning)
    {
        isRunning = true;
    }
    else if(!robot.autoActivated && isRunning)
    {
        currentState = State::wait;
        robot.init();
        robot.setPosition(0);
        firstLoop = true;
        isRunning = false;
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
                currentState = State::approach;
            }

            break;

        case State::approach:
            toPosition = robot.approachPosition;
            fromStateStopPendulum = false;

            if (robot.moveForward(robot.approachSpeed, toPosition))
            {
                currentState = State::miniReverse;
            }

            break;

        case State::miniReverse:
            toPosition = robot.approachPosition-0.1;
            fromStateStopPendulum = false;

            if (robot.moveReverse(robot.fastSpeed, toPosition))
            {
                currentState = State::forward;
            }

            break;

        case State::forward:
            toPosition = robot.endPosition - 0.10;
            if (robot.moveForward(0.8, toPosition))
            {
                currentState = State::forwardAfterObstacle;
            }
            break;

        case State::forwardAfterObstacle:
            toPosition = robot.endPosition;
            if (robot.moveForward(0.30*robot.fastSpeed, toPosition, robot.dropPosition)) {
                currentState = State::reverse;
                robot.disableMagnet();
            }
            break;

        case State::reverse:
            toPosition = 0.60;
            robot.moveReverse(robot.fastSpeed, toPosition) ? currentState = State::stopPendulum : currentState;
            break;

        case State::stopPendulum:

            if(!pidEnabled)
            {
                timerPID.tic();
                pid_.enable();
                pidEnabled = true;
                delay(100);
            }
            else
            {
                if(timerPID.toc() > 2000)
                {
                    pid_.disable();
                    pidEnabled = false;
                    currentState = State::reverseIntoWall;
                    timerPID.reset();
                }
                pid_.run();
            }
            break;

        case State::reverseIntoWall:
            toPosition = -0.05;
            if(robot.moveReverse(0.3, toPosition))
            {
                currentState = State::approach;
                robot.init();
                robot.setSpeed(0);
                robot.enableMagnet();
                robot.setPosition(0);
                delay(2000);
            }
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