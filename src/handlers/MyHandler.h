#ifndef MYHANDLER_H
#define MYHANDLER_H

#include "common/MyGlobal.h"

typedef MyState (*EventHandler)(const MyEvent *event);

extern EventHandler state_handler_table[STATE_MAX][EVT_MAX];

void init_state_handler_table();

// Shared Helper Functions
void connectWifiAP();
void firmwareUpdate();

#endif // MYHANDLER_H
