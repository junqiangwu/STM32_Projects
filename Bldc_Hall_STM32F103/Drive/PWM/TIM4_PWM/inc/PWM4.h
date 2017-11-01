#ifndef _PWM4_H
#define _PWM4_H

extern void  PWM4_GPIOConfig(void);
extern void  PWM4_Config(void);
extern void  PWM4_OC1Start(void);
extern void  PWM4_OC2Start(void);
extern void  PWM4_OC3Start(void);
extern void  PWM4_OC4Start(void);
extern void  PWM4_OC1Stop(void);
extern void  PWM4_OC2Stop(void);
extern void  PWM4_OC3Stop(void);
extern void  PWM4_OC4Stop(void);
extern void  PWM4_SetOC1Pulse(uint16_t pulse);
extern void  PWM4_SetOC2Pulse(uint16_t pulse);
extern void  PWM4_SetOC3Pulse(uint16_t pulse);
extern void  PWM4_SetOC4Pulse(uint16_t pulse);
#endif
