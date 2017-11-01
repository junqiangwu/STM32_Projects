#ifndef _PWM_H
#define _PWM_H

extern void  PWM3_GPIOConfig(void);
extern void  PWM3_Config(void);
extern void  PWM3_OC1Start(void);
extern void  PWM3_OC2Start(void);
extern void  PWM3_OC1Stop(void);
extern void  PWM3_OC2Stop(void);
extern void  SetOC1Pulse(uint16_t pulse);
extern void  SetOC2Pulse(uint16_t pulse);
#endif
