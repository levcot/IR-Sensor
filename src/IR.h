
#ifndef IR_H
#define IR_H

#include <Arduino.h>

#define IR_SENSOR_COUNT 11

void IR_begin();

void IR_calibrate(unsigned long durationMs = 5000);

int IR_getPosition();

void IR_readRaw(uint16_t values[IR_SENSOR_COUNT]);

bool IR_isLineDetected();

#endif
