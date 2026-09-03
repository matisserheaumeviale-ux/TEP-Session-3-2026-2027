#include "adc.h"

typedef struct
{
    volatile uint32_t CR;
    volatile uint32_t SR;
    volatile uint32_t DR;
} ADC_TypeDef;

static ADC_TypeDef adcSimule = {0};

#define ADC1 (&adcSimule)

void adc_init(void)
{
    ADC1->CR |= (1 << 0);
}

uint16_t adc_lire(uint8_t canal)
{
    ADC1->CR = (ADC1->CR & ~0xFF00) | (canal << 8);
    ADC1->CR |= (1 << 1);

    return 1200 + canal * 50;
}