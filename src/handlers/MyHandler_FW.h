#ifndef MYHANDLER_FW_H
#define MYHANDLER_FW_H

#include "MyHandler.h"

MyState handler_fw_nop(const MyEvent *event);
MyState handler_fw_updating(const MyEvent *event);
MyState handler_fw_ble_disconnected(const MyEvent *event);

#endif // MYHANDLER_FW_H
