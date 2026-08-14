#ifndef STATEWAIT_H
#define STATEWAIT_H

#include "states/State.h"

class StateWait : public State
{
public:
  MyState getStateID() const override { return STATE_WAIT; }
  void onEnter() override;
  MyState handleEvent(const MyEvent *event) override;
};

#endif // STATEWAIT_H
