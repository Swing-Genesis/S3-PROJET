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

    pid_.setGains(0.20, 0 , 0.07);
    // Attache des fonctions de retour
    pid_.setMeasurementFunc(PIDmeasurement);
    pid_.setCommandFunc(PIDcommand);
    //pid_.setAtGoalFunc(PIDgoalReached);
    pid_.setEpsilon(0.001);
    pid_.setPeriod(200);
    pid_.setGoal(0);
    pid_.enable();
}

void loop()
{
    bool checkMR;
    double to_position = 0;
    String tocString;
    String tspString;
    //currentState = State::PIDtest;
    if (digitalRead(LEFT_BUTTON))
    {
        if (isRunning) {
            isRunning = false;
        }         
        else {
            isRunning = true;
        }
            
        delay(500);
    }

    // if (shouldRead_)
    // {
    //     robot.readJSON(shouldRead_);
    // }

    if (!MANETTE)
    {
        switch (currentState)
        {
        case State::wait:
            robot.enableMagnet();
            Serial.println("st_wait");
            //robot.moveReverse(-1, 0) ? currentState = State::forward : currentState;
            
            if (isRunning)
            {
                
                if (robot.moveReverse(-0.1, 0))
                { 
                    //Serial.println(robot.getPosition());
                    currentState = State::initReverse;
                    
                }
                delay(1000);
            }
            
            //isRunning ? currentState = State::initReverse : currentState;
            break;

        case State::initReverse:
            Serial.println("st_initrev");
            to_position = robot.init_reverse_position;
            fromStateStopPendulum = false;
            checkMR = robot.moveReverse (1, to_position);
            
            if (checkMR)
            {
               // Serial.println(currentState);
                currentState = State::forward;
                //Serial.println(currentState);
            }
            //Serial.println(robot.getPosition());
            break;

        case State::forward:
            Serial.println("st_forward");
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
            Serial.println("st_reverse");
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

                if (!pidEnabled) {
                    timerPID.tic();
                    pidEnabled = true;
                    pid_.enable();
                    delay(100);
                }
                
                if(pidEnabled){
                    pid_.run();
                } 

                // prints timerPID.toc() for debugging
                //tocString = String(timerPID.toc(), 4);
                //tspString = String(robot.time_stop_pendulum, 4);
                //Serial.println("Toc : " + tocString);
                //Serial.println("Time stop pendulum : " + tspString);
                if (timerPID.toc() > robot.time_stop_pendulum)
                {

                    pid_.disable();
                    pidEnabled = false;
                    timerPID.reset();
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
            Serial.println("st_emergency");
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
}

// Fonctions pour le PID
double PIDmeasurement(){

  float analogValue = analogRead(POTENTIOMETER_PIN);
  double pendulumAngle = (Helpers::floatMap(analogValue, 170, 960, -180, 180)+3);
  return pendulumAngle;
}

// the cmd calculated by compute command is then used with a weight to lower or augment speed
void PIDcommand(double cmd){
  cmdCheck = cmd*0.075;
  robot.setSpeed(cmdCheck*0.35);
}

void PIDgoalReached(double pendulumAngle){
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
          return; //valeur pour dire d<arreter PID
      }
    }
}

void timerCallback() { shouldSend_ = true; }

void serialEvent() { shouldRead_ = true; }