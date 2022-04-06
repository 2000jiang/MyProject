#ifndef IR_PROTOOCOLS_H
#define IR_PROTOOCOLS_H

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "ir_tools.h"

#include"motor.h"

//void ir_tx_task(void *arg);
void ir_rx_task(void *arg);

#endif