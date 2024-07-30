#ifndef __STATES_HPP__
#define __STATES_HPP__

enum Mode
{
  autonomous = 0,
  manual = 1,
  none = 2
};

enum State
{
  wait = 0,
  initReverse = 1,
  forward = 2,
  reverse = 3,
  stopPendulum = 4,
  emergencyStop = 5,
  PIDtest = 6
};

#endif //__STATES_HPP__


