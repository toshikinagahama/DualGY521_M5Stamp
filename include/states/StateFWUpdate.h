#ifndef STATEFWUPDATE_H
#define STATEFWUPDATE_H

#include "states/State.h"

class StateFWUpdate : public State
{
public:
  MyState getStateID() const override { return STATE_FW_UPDATE; }
  void onEnter() override;
  MyState handleEvent(const MyEvent *event) override;
};

class StateFWUpdating : public State
{
public:
  MyState getStateID() const override { return STATE_FW_UPDATING; }
  void onEnter() override;
  MyState handleEvent(const MyEvent *event) override;
};

#endif // STATEFWUPDATE_H
