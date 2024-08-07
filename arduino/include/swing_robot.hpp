#ifndef __SWING_ROBOT_HPP__
#define __SWING_ROBOT_HPP__

#include "LibS3GRO.h"
#include "helpers.hpp"
#include "config.hpp"
#include "states_modes.hpp"

class SwingRobot
{
private:
    /*
    The current solution with static functions and a static instance pointer is indeed not ideal.
    It's a workaround for a limitation in the design of the PID class,
    which is expecting C-style function pointers instead of more flexible callable types.
    */
    static SwingRobot *instance_;

    /*----- Composants -----*/
    ArduinoX AX_;
    VexQuadEncoder vexEncoder_;
    IMU9DOF imu_;

    /*----- Caractéristiques -----*/
    float position_;
    bool isMagnetON_ = false;
    float travelledDistance_ = 0.0;

    /*----- PID -----*/
    PID pid_;
    float cmdCheck_;

    void initPID()
    {
        // Initialisation du PID
        pid_.setGains(0.20, 0, 0.07);

        // Attache des fonctions de retour
        pid_.setMeasurementFunc(&SwingRobot::PIDmeasurement);
        pid_.setCommandFunc(&SwingRobot::PIDcommand);
        pid_.setAtGoalFunc(&SwingRobot::PIDgoalReached);

        pid_.setEpsilon(0.001);
        pid_.setPeriod(200);
    }

    static void isrWrapper()
    {
        if (instance_)
        {
            instance_->vexEncoder_.isr();
        }
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
        vexEncoder_.init(2, 3);
        attachInterrupt(vexEncoder_.getPinInt(), isrWrapper, FALLING);

        initPID();

        AX_.setMotorPWM(0, 0);
        AX_.setMotorPWM(1, 0);
    }

    void enablePID()
    {
        pid_.enable();
    }

    void disablePID()
    {
        pid_.disable();
    }

    static double PIDmeasurement()
    {
        float analogValue = analogRead(POTENTIOMETER_PIN);
        double pendulumAngle = (Helpers::floatMap(analogValue, 170, 960, -180, 180) + 3); // 3 est un OFFSET ?
        return pendulumAngle;
    }

    static void PIDcommand(double cmd)
    {
        SwingRobot *instance = SwingRobot::instance_;

        instance->cmdCheck_ = cmd * 0.075;
        instance->AX_.setMotorPWM(0, instance->cmdCheck_ * 0.35);
    }

    static void PIDgoalReached()
    {
        // TODO
    }

    void setSpeed(float speed_)
    {
        AX_.setMotorPWM(0, speed_);
    }

    bool moveForward(float speed_, float toPosition__)
    {
        while (position_ < toPosition__)
        {
            position_ = Helpers::tickToMeters(AX_.readEncoder(0));
            AX_.setMotorPWM(0, speed_);
        }
        return true;
    }

    bool moveForward(float speed_, float toPosition__, float dropPosition__)
    {
        while (position_ < toPosition__)
        {
            if (position_ > dropPosition__)
            {
                isMagnetON_ = false;
            }
            position_ = Helpers::tickToMeters(AX_.readEncoder(0));
            AX_.setMotorPWM(0, speed_);
        }
        isMagnetON_ = true;
        return true;
    }

    bool moveReverse(float speed_, float toPosition__)
    {
        while (position_ > toPosition__)
        {
            position_ = Helpers::tickToMeters(AX_.readEncoder(0));
            AX_.setMotorPWM(0, speed_);
        }
        return true;
    }

    double getPosition()
    {
        return position_;
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
        return isMagnetON_;
    }

    void sendJSON(bool &shouldSend_)
    {

        JsonDocument doc;
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

        unsigned long encoderValue = AX_.readEncoder(0);

        doc["TravelledDistance"] = Helpers::tickToMeters(encoderValue);

        Serial.print("Meters : ");
        Serial.println(Helpers::tickToMeters(encoderValue));

        // Serialisation
        serializeJson(doc, Serial);
        // Envoit
        Serial.println();
        shouldSend_ = false;
    }

    void readJSON(bool &shouldRead_)
    {
        // Lecture du message Json
        JsonDocument doc;
        JsonVariant parse_msg;

        // Lecture sur le port Seriel
        DeserializationError error = deserializeJson(doc, Serial);
        shouldRead_ = false;

        // Si erreur dans le message
        if (error)
        {
            Serial.print("deserialize() failed: ");
            Serial.println(error.c_str());
            return;
        }

        parse_msg = doc["setGoal"];
        if (!parse_msg.isNull())
        {
            pid_.disable();
            pid_.setGains(doc["setGoal"][0], doc["setGoal"][1], doc["setGoal"][2]);
            pid_.setEpsilon(doc["setGoal"][3]);
            pid_.setGoal(doc["setGoal"][4]);
            pid_.enable();
        }

        parse_msg = doc["magnet"];
        if (!parse_msg.isNull())
        {
            isMagnetON_ = doc["magnet"].as<bool>();
        }

        
    }
};

// Initialize static member
SwingRobot *SwingRobot::instance_ = nullptr;

#endif //__SWING_ROBOT_HPP__