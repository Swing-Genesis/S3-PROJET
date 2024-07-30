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
    bool pidEnabled_ = false;
    float cmdCheck_;



    void initPID()
    {
        // Initialisation du PID
        pid_.setGains(kp, ki, kd);

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

    // Position / vitesses
    float init_reverse_position = -0.1;
    float drop_position = 0.6;
    float end_position = 0.8;
    float slow_speed = 0.1;
    float fast_speed = 0.9;
    float time_stop_pendulum = 3000;

    float kp = 0.20;
    float ki = 0;
    float kd = 0.07;

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
        pidEnabled_ = true;
    }

    void disablePID()
    {
        pid_.disable();
        pidEnabled_ = false;
    }

    void runPID()
    {
        pid_.run();
    }

    bool getPIDState()
    {
        return pidEnabled_;
    }

    static double PIDmeasurement()
    {
        printf("PIDmeasurement\n");
        float analogValue = analogRead(POTENTIOMETER_PIN);
        double pendulumAngle = (Helpers::floatMap(analogValue, 170, 960, -180, 180) + 3); // 3 est un OFFSET ?
        return pendulumAngle;
    }

    static void PIDcommand(double cmd)
    {
        printf("cmd : %f\n", cmd);
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

    bool moveForward(float speed_, float toPosition__, float drop_position__)
    {
        while (position_ < toPosition__)
        {
            if (position_ > drop_position__)
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
            Serial.println("QT sent setGoal");  
            pid_.disable();
            pid_.setGains(doc["setGoal"][0], doc["setGoal"][1], doc["setGoal"][2]);
            pid_.setEpsilon(doc["setGoal"][3]);
            pid_.setGoal(doc["setGoal"][4]);
            pid_.enable();
        }

        parse_msg = doc["magnet"];
        if (!parse_msg.isNull())
        {
            Serial.println("QT changed magnet state");
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

        parse_msg = doc["slow_speed"];
        if (!parse_msg.isNull())
        {   
            Serial.println("QT changed slow_speed");
            float slow_speed_valid = doc["slow_speed"].as<float>();
            if (slow_speed_valid > 0 && slow_speed_valid < fast_speed)
            {
                slow_speed = doc["slow_speed"];
            }
        }

        parse_msg = doc["fast_speed"];
        if (!parse_msg.isNull())
        {
            Serial.println("QT changed fast_speed");
            float fast_speed_valid = doc["fast_speed"].as<float>();
            if (fast_speed_valid > 0 && fast_speed_valid > slow_speed)
            {
                fast_speed = doc["fast_speed"];
            }
        }

        parse_msg = doc["drop_position"];
        if (!parse_msg.isNull())
        {
            Serial.println("QT changed drop_position");
            float drop_position_valid = doc["drop_position"].as<float>();
            if (drop_position_valid > 0 && drop_position_valid < end_position)
            {
                drop_position = doc["drop_position"];
            }
            else
            {
                drop_position = end_position;
            }
        }

        parse_msg = doc["end_position"];
        if (!parse_msg.isNull())
        {
            Serial.println("QT changed end_position");
            float end_position_valid = doc["end_position"].as<float>();
            if (end_position_valid > 0 && end_position_valid > drop_position)
            {
                end_position = doc["end_position"];
            }
            else
            {
                end_position = drop_position;
            }
        }   

        parse_msg = doc["init_reverse_position"];
        if (!parse_msg.isNull())
        {
            Serial.println("QT changed init_reverse_position");
            float init_reverse_position_valid = doc["init_reverse_position"].as<float>();
            if (init_reverse_position_valid < 0 && init_reverse_position_valid > -0.2)
            {
                init_reverse_position = doc["init_reverse_position"];
            }
        }  

        parse_msg = doc["time_stop_pendulum"];
        if (!parse_msg.isNull())
        {
            Serial.println("QT changed time_stop_pendulum");
            float time_stop_pendulum_valid = doc["time_stop_pendulum"].as<float>();
            if (time_stop_pendulum_valid > 0)
            {
                time_stop_pendulum = doc["time_stop_pendulum"];
            }
        }
        
    }
};

// Initialize static member
SwingRobot *SwingRobot::instance_ = nullptr;

#endif //__SWING_ROBOT_HPP__