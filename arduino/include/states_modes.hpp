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
  approach = 1,
  miniReverse = 2,
  forward = 3,
  forwardAfterObstacle = 4,
  reverse = 5,
  stopPendulum = 6,
  reverseIntoWall = 7,
  emergencyStop = 8,
  PIDtest = 9
};

#endif //__STATES_HPP__


