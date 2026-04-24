#ifndef __KEY_H
#define __KEY_H

#include "all.h"

void KeyScan(void);
uint8_t GetKey(void);

extern volatile uint8_t Key_Data;

#endif
