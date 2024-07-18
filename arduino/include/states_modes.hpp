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
  st_wait = 0,
  st_fwaccelerate = 1,
  st_forward = 2,
  st_decelerate = 3,
  st_revaccelerate = 4,
  st_reverse = 5,
  st_stoppendulum = 6
};

#endif //__STATES_HPP__


