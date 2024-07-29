/* 
 * GRO 302 - Conception d'un robot mobile
 * Code de démarrage
 * Auteurs: Jean-Samuel Lauzon     
 * date: 1 mai 2019
*/

/*------------------------------ Librairies ---------------------------------*/
#include <LibS3GRO.h>
#include <ArduinoJson.h>

#include <swing_robot.hpp>
#include <json_manager.hpp>
#include <config.hpp>
#include <states_modes.hpp>

/*---------------------------- variables globales ---------------------------*/

SwingRobot robot;

SoftTimer timerSendMsg_;
bool shouldRead_ = false;
bool shouldSend_ = false;

void timerCallback();
void serialEvent();

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
    if(shouldSend_)
    {
        robot.sendJSON(shouldSend_);
    }

    if(shouldRead_)
    {
        digitalWrite(LED_BUILTIN,HIGH);
        robot.readJSON(shouldRead_);
    }

    if(robot.getMagnetState())
    {
        robot.enableMagnet();
    }
    else
    {
        robot.disableMagnet();
    }

    timerSendMsg_.update();

    robot.setSpeed(25.0); 
}

void timerCallback() {shouldSend_ = true;}

void serialEvent(){shouldRead_ = true;}