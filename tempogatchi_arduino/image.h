#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif

//hello
extern PROGMEM const unsigned char gImage_hello1[];
extern PROGMEM const unsigned char gImage_hello2[];

//HOT
extern PROGMEM const unsigned char gImage_hot1[];
extern PROGMEM const unsigned char gImage_hot2[];

//COLD
extern PROGMEM const unsigned char gImage_cold1[];
extern PROGMEM const unsigned char gImage_cold2[];
extern PROGMEM const unsigned char gImage_cold3[];

#ifdef __cplusplus
}
#endif

#endif