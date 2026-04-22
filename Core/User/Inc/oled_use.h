#ifndef __OLED_USE_H
#define __OLED_USE_H

#include "all.h"

void OLED_INITSHOW(void);
void OLED_INMATIONSHOW(void);
void OLED_READSHOW(void);
void OLED_SETSHOW(void);
void OLED_SETSHOWMAX(void);
void OLED_SETSHOWMIN(void);

extern uint8_t threshold_value;

#endif
