#ifndef SWITCH_H
#define SWITCH_H

void initSwitchINT0(void);
void enableSwitchInterrupt(void);
void disableSwitchInterrupt(void);
unsigned char switchPressed(void);
void clearSwitchInterruptFlag(void);

#endif