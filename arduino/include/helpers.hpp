#ifndef HELPERS_HPP
#define HELPERS_HPP


namespace Helpers
{
    float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }


    double tickToMeters(int ticks_)
    {
        return ((static_cast<double>(ticks_) / 3200.0) * 0.1 * PI);
    }
} 

#endif // HELPERS_H