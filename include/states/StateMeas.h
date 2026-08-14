#ifndef STATEMEAS_H
#define STATEMEAS_H

#include "states/State.h"

class StateMeas : public State
{
public:
  MyState getStateID() const override { return STATE_MEAS; }
  void onEnter() override;
  void onExit() override;
  MyState handleEvent(const MyEvent *event) override;
};

#endif // STATEMEAS_H
