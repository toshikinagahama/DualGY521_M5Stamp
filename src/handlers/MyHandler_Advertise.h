#ifndef MYHANDLER_ADVERTISE_H
#define MYHANDLER_ADVERTISE_H

#include "MyHandler.h"

MyState handler_adv_nop(const MyEvent *event);
MyState handler_adv_ble_connected(const MyEvent *event);
MyState handler_adv_ble_disconnected(const MyEvent *event);

#endif // MYHANDLER_ADVERTISE_H
