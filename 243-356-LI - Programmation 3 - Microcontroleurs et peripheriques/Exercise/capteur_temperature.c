#include <stdint.h>

#include "adc.h"
#include "capteur.h"
#include "capteur_temperature.h"

typedef struct
{
    uint8_t canal;
    int16_t dernierEchantillon;
    EtatCapteur etat;
} CapteurTemperature;

static CapteurTemperature temperature =
{
    .canal = ADC_CANAL_TEMPERATURE
};

void temperature_init(void)
{
    temperature.canal = ADC_CANAL_TEMPERATURE;
    temperature.etat = CAPTEUR_OK;
}

float temperature_lire_celsius(void)
{
    uint16_t brut = adc_lire(temperature.canal);

    temperature.dernierEchantillon = (int16_t)brut;

    return (float)brut / 40.0f;
}