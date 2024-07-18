#ifndef __SWING_ROBOT_HPP__
#define __SWING_ROBOT_HPP__

#include <LibS3GRO.h>

#include <json_manager.hpp>
#include <config.hpp>
#include <states_modes.hpp>

class SwingRobot
{
private:
    static SwingRobot* instance_;

    /*----- Composants -----*/
    ArduinoX AX_;
    JsonManager jsonManager();
    VexQuadEncoder vexEncoder_;
    IMU9DOF imu_;
    PID pid_;

    /*----- Caractéristiques -----*/
    State currentState;
    double position;
    bool isMagnetON = false;

    /*----- Synchonisation du JSON -----*/

    
    

    void initPID()
    {
        // Initialisation du PID
        pid_.setGains(0.25,0.1 ,0);
        // Attache des fonctions de retour
        pid_.setMeasurementFunc(PIDmeasurement);
        pid_.setCommandFunc(PIDcommand);
        pid_.setAtGoalFunc(PIDgoalReached);
        pid_.setEpsilon(0.001);
        pid_.setPeriod(200);
    }

    struct dataToSend {
        unsigned long time;
        int potVex;
        double goal;
        double measurements;
    } dataToSend;

    // Fonctions pour le PID
    static double PIDmeasurement(){
    // To do
    }
    static void PIDcommand(double cmd){
    // To do
    }
    static void PIDgoalReached(){
    // To do
    }

    static void isrWrapper() {
        if (instance_) {
            instance_->vexEncoder_.isr();
        }
    }

    static double tickToMeters(int ticks_)
    {
        return ((ticks_/3200.0)*0.1*PI);
    }

public:

    SwingRobot()
    {
        instance_ = this;
    }

    void init()
    {
        Serial.println("Initializing SwingRobot...");
        AX_.init();
        imu_.init(); 
        vexEncoder_.init(2,3);
        attachInterrupt(vexEncoder_.getPinInt(), isrWrapper, FALLING);

        initPID();

        AX_.setMotorPWM(0, 0);
        AX_.setMotorPWM(1, 0);
    }

    void setSpeed(float speed_)
    {
        AX_.setMotorPWM(0, speed_);
    }

    void enableMagnet()
    {
        digitalWrite(MAGNET_PIN, HIGH);
    }

    void disableMagnet()
    {
        digitalWrite(MAGNET_PIN, LOW);
    }

    bool getMagnetState()
    {
        return isMagnetON;
    }

    void sendJSON(bool& shouldSend_){
        
        StaticJsonDocument<500> doc;
        // Elements du message

        doc["time"] = millis();
        doc["potVex"] = analogRead(POTENTIOMETER_PIN);
        doc["encVex"] = vexEncoder_.getCount();
        doc["goal"] = pid_.getGoal();
        doc["measurements"] = PIDmeasurement();
        doc["voltage"] = AX_.getVoltage();
        doc["current"] = AX_.getCurrent(); 
        doc["accelX"] = imu_.getAccelX();
        doc["accelY"] = imu_.getAccelY();
        doc["accelZ"] = imu_.getAccelZ();
        doc["gyroX"] = imu_.getGyroX();
        doc["gyroY"] = imu_.getGyroY();
        doc["gyroZ"] = imu_.getGyroZ();
        doc["isGoal"] = pid_.isAtGoal();
        doc["actualTime"] = pid_.getActualDt();

        // Serialisation
        serializeJson(doc, Serial);
        // Envoit
        Serial.println();
        shouldSend_ = false;
    }

    void readJSON(bool& shouldRead_)
    {
        // Lecture du message Json
        StaticJsonDocument<500> doc;
        JsonVariant parse_msg;

        // Lecture sur le port Seriel
        DeserializationError error = deserializeJson(doc, Serial);
        shouldRead_ = false;

        // Si erreur dans le message
        if (error) {
            Serial.print("deserialize() failed: ");
            Serial.println(error.c_str());
            return;
        }

        parse_msg = doc["setGoal"];
        if(!parse_msg.isNull()){
            pid_.disable();
            pid_.setGains(doc["setGoal"][0], doc["setGoal"][1], doc["setGoal"][2]);
            pid_.setEpsilon(doc["setGoal"][3]);
            pid_.setGoal(doc["setGoal"][4]);
            pid_.enable();
        }

        parse_msg = doc["magnet"];
        if(!parse_msg.isNull()){
            isMagnetON = doc["magnet"].as<bool>();
        }
    }
    
};

// Initialize static member
SwingRobot* SwingRobot::instance_ = nullptr;



#endif //__SWING_ROBOT_HPP__