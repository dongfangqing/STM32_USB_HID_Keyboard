#ifndef __GPIO_PIN_H
#define __GPIO_PIN_H

#include "stm32f4xx_hal.h"

typedef struct
{
    GPIO_TypeDef* Port;
    uint16_t Pin;
} GPIOPin;

#endif
