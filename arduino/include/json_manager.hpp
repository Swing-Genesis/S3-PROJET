#ifndef __JSON_MANAGER_HPP__
#define __JSON_MANAGER_HPP__

#include <LibS3GRO.h>
#include <ArduinoJson.h>

#include <config.hpp>

class JsonManager
{
private:

    SoftTimer timerSendMsg_;
    SoftTimer timerPulse_;

public:
    JsonManager()
    {
    }

    void sendMsg()
    {
        
    }

    void readMsg()
    {
        
    }
};

#endif