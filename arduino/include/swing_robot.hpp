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
    PID pid_;

    /*----- Caractéristiques -----*/
    double position_;
    bool isMagnetON_ = false;
    float travelledDistance_ = 0.0f;
    float oldJSONtime = 0.0f;


    // void initPID()
    // {
    //     // Initialisation du PID
    //     pid_.setGains(kp, ki, kd);

    //     // Attache des fonctions de retour
    //     pid_.setMeasurementFunc(&SwingRobot::PIDmeasurement);
    //     pid_.setCommandFunc(&SwingRobot::PIDcommand);
    //     pid_.setAtGoalFunc(&SwingRobot::PIDgoalReached);

    //     pid_.setEpsilon(0.001);
    //     pid_.setPeriod(200);
    // }

    static void isrWrapper()
    {
        if (instance_)
        {
            instance_->vexEncoder_.isr();
        }
    }

public:

    // Position / vitesses
    // 1 sapin : end=0.80
    // 2 sapin : end=0.85
    float approachPosition = 0.40;
    float approachSpeed = 0.4;
    float dropPosition = 0.91;
    float endPosition = 1.00;
    float slowSpeed = 0.1;
    float fastSpeed = 0.9;
    float timeStopPendulum = 3000;

    bool autoActivated = false;

    float kp = 0.20;
    float ki = 0;
    float kd = 0.07;

    SwingRobot(PID pid)
    {
        pid_ = pid;
        instance_ = this;
    }

    void init()
    {
        AX_.init();
        imu_.init();
        vexEncoder_.init(2, 3);
        attachInterrupt(vexEncoder_.getPinInt(), isrWrapper, FALLING);

        //initPID();

        AX_.setMotorPWM(0, 0);
        AX_.setMotorPWM(1, 0);
        AX_.resetEncoder(0);
    }

    float getTravelledDistance()
    {
        return travelledDistance_; 
    }

    void setPosition(float position)
    {
        this->position_ = position;
    }

    void setSpeed(float speed_)
    {
        AX_.setMotorPWM(0, speed_);
    }

    bool moveForward(float speed_, float toPosition_)
    {
        if(position_ < toPosition_)
        {
            position_ = Helpers::tickToMeters(AX_.readEncoder(0));
            AX_.setMotorPWM(0, speed_);
            return false;
        }
        return true;
    }

    bool moveForward(float speed_, float toPosition_, float dropPosition_)
    {
        if(position_ < toPosition_)
        {
            if (position_ > dropPosition_)
            {
                this->disableMagnet();
            }
            position_ = Helpers::tickToMeters(AX_.readEncoder(0));
            AX_.setMotorPWM(0, speed_);
            return false;
        }
        this->enableMagnet();
        return true;
    }

    bool moveReverse(float speed_, float toPosition_)
    {
        if(position_ > toPosition_)
        {
            position_ = Helpers::tickToMeters(AX_.readEncoder(0));
            AX_.setMotorPWM(0, -speed_);
            return false;
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
        //doc["measurements"] = PIDmeasurement();
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

        float deltaTime = (millis()/1000.0f) - oldJSONtime;
        travelledDistance_ = travelledDistance_ + abs(deltaTime*position_);
        oldJSONtime = (millis()/1000.0f);

        doc["TravelledDistance"] = travelledDistance_;
        doc["position"] = position_;

        //Serial.print("Meters : ");
        //Serial.println(Helpers::tickToMeters(encoderValue));

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

        parse_msg = doc["magnet"];
        if (!parse_msg.isNull())
        {
            if (doc["magnet"].as<bool>())
            {
                enableMagnet();
            }
            else
            {
                disableMagnet();
            }
        }

        parse_msg = doc["Ki"];
        if (!parse_msg.isNull())
        {   
            ki = doc["Ki"].as<float>();
        }

        parse_msg = doc["Kp"];
        if (!parse_msg.isNull())
        {   
            kp = doc["Kp"].as<float>();
        }

        parse_msg = doc["Kd"];
        if (!parse_msg.isNull())
        {   
            kd = doc["Kd"].as<float>();
        }

        pid_.setGains(kp,ki,kd);

        parse_msg = doc["slowSpeed"];
        if (!parse_msg.isNull())
        {
            slowSpeed = doc["slowSpeed"];
        }

        parse_msg = doc["fastSpeed"];
        if (!parse_msg.isNull())
        {
            fastSpeed = doc["fastSpeed"][0];
        }

        parse_msg = doc["dropPosition"];
        if (!parse_msg.isNull())
        {
            dropPosition = doc["dropPosition"];
        }

        parse_msg = doc["endPosition"];
        if (!parse_msg.isNull())
        {
            endPosition = doc["endPosition"];
        }   

        parse_msg = doc["initReversePosition"];
        if (!parse_msg.isNull())
        {
            approachPosition = doc["initReversePosition"];
        }  

        parse_msg = doc["timeStopPendulum"];
        if (!parse_msg.isNull())
        {
            timeStopPendulum = doc["timeStopPendulum"];
        }

        parse_msg = doc["startAuto"];
        if (!parse_msg.isNull())
        {
            autoActivated = doc["startAuto"].as<bool>();     
        }
        
    }
};
// Initialize static member
SwingRobot *SwingRobot::instance_ = nullptr;

#endif //__SWING_ROBOT_HPP__