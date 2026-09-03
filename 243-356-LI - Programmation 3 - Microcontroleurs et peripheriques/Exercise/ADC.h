#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define ADC_CANAL_TEMPERATURE 0
#define ADC_CANAL_HUMIDITE    1

void adc_init(void);
uint16_t adc_lire(uint8_t canal);

#endif