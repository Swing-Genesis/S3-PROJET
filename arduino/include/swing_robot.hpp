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
    float travelledDistance_ = 0.0;


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
    float initReversePosition = -0.1;
    float dropPosition = 0.6;
    float endPosition = 0.8;
    float slowSpeed = 0.1;
    float fastSpeed = 0.9;
    float timeStopPendulum = 3000;

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
        Serial.println("Initializing SwingRobot...");
        AX_.init();
        imu_.init();
        vexEncoder_.init(2, 3);
        attachInterrupt(vexEncoder_.getPinInt(), isrWrapper, FALLING);

        //initPID();

        AX_.setMotorPWM(0, 0);
        AX_.setMotorPWM(1, 0);
        AX_.resetEncoder(0);
    }

    // void enablePID()
    // {
    //     pid_.enable();
    //     pidEnabled_ = true;
    // }

    // void disablePID()
    // {
    //     pid_.disable();
    //     pidEnabled_ = false;
    // }

    // void runPID()
    // {
    //     pid_.run();
    // }

    // bool getPIDState()
    // {
    //     return pidEnabled_;
    // }

    // static double PIDmeasurement()
    // {
    //     printf("PIDmeasurement\n");
    //     float analogValue = analogRead(POTENTIOMETER_PIN);
    //     double pendulumAngle = (Helpers::floatMap(analogValue, 170, 960, -180, 180) + 3); // 3 est un OFFSET ?
    //     return pendulumAngle;
    // }

    // static void PIDcommand(double cmd)
    // {
    //     printf("cmd : %f\n", cmd);
    //     SwingRobot *instance = SwingRobot::instance_;

    //     instance->cmdCheck_ = cmd * 0.075;
    //     instance->AX_.setMotorPWM(0, instance->cmdCheck_ * 0.35);
    // }

    // static void PIDgoalReached()
    // {
    //     // TODO
    // }

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

        doc["TravelledDistance"] = Helpers::tickToMeters(encoderValue);

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

        parse_msg = doc["setGoal"];
        if (!parse_msg.isNull())
        {
            //Serial.println("QT sent setGoal");  
            pid_.disable();
            pid_.setGains(doc["setGoal"][0], doc["setGoal"][1], doc["setGoal"][2]);
            pid_.setEpsilon(doc["setGoal"][3]);
            pid_.setGoal(doc["setGoal"][4]);
            pid_.enable();
        }

        parse_msg = doc["magnet"];
        if (!parse_msg.isNull())
        {
            //Serial.println("QT changed magnet state");
            // use doc["magnet"].as<bool>();
            if (doc["magnet"].as<bool>())
            {
                enableMagnet();
            }
            else
            {
                disableMagnet();
            }
        }

        parse_msg = doc["slowSpeed"];
        if (!parse_msg.isNull())
        {   
            //Serial.println("QT changed slowSpeed");
            float slowSpeed_valid = doc["slowSpeed"].as<float>();
            if (slowSpeed_valid > 0 && slowSpeed_valid < fastSpeed)
            {
                slowSpeed = doc["slowSpeed"];
            }
        }

        parse_msg = doc["fastSpeed"];
        if (!parse_msg.isNull())
        {
            //Serial.println("QT changed fastSpeed");
            float fastSpeed_valid = doc["fastSpeed"].as<float>();
            if (fastSpeed_valid > 0 && fastSpeed_valid > slowSpeed)
            {
                fastSpeed = doc["fastSpeed"];
            }
        }

        parse_msg = doc["dropPosition"];
        if (!parse_msg.isNull())
        {
            //Serial.println("QT changed dropPosition");
            float dropPosition_valid = doc["dropPosition"].as<float>();
            if (dropPosition_valid > 0 && dropPosition_valid < endPosition)
            {
                dropPosition = doc["dropPosition"];
            }
            else
            {
                dropPosition = endPosition;
            }
        }

        parse_msg = doc["endPosition"];
        if (!parse_msg.isNull())
        {
            //Serial.println("QT changed endPosition");
            float endPosition_valid = doc["endPosition"].as<float>();
            if (endPosition_valid > 0 && endPosition_valid > dropPosition)
            {
                endPosition = doc["endPosition"];
            }
            else
            {
                endPosition = dropPosition;
            }
        }   

        parse_msg = doc["initReversePosition"];
        if (!parse_msg.isNull())
        {
            //Serial.println("QT changed initReversePosition");
            float initReversePosition_valid = doc["initReversePosition"].as<float>();
            if (initReversePosition_valid < 0 && initReversePosition_valid > -0.2)
            {
                initReversePosition = doc["initReversePosition"];
            }
        }  

        parse_msg = doc["timeStopPendulum"];
        if (!parse_msg.isNull())
        {
            //Serial.println("QT changed timeStopPendulum");
            float timeStopPendulum_valid = doc["timeStopPendulum"].as<float>();
            if (timeStopPendulum_valid > 0)
            {
                timeStopPendulum = doc["timeStopPendulum"];
            }
        }
        
    }
};
// Initialize static member
SwingRobot *SwingRobot::instance_ = nullptr;

#endif //__SWING_ROBOT_HPP__