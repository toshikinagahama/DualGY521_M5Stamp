#ifndef MYHANDLER_MEAS_H
#define MYHANDLER_MEAS_H

#include "MyHandler.h"

MyState handler_meas_nop(const MyEvent *event);
MyState handler_meas_cmd_meas_stop(const MyEvent *event);
MyState handler_meas_ble_disconnected(const MyEvent *event);

#endif // MYHANDLER_MEAS_H
