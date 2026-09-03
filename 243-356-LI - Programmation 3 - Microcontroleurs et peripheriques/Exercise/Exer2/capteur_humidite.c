#include <stdint.h>

#include "adc.h"
#include "capteur.h"
#include "capteur_humidite.h"

typedef struct
{
    uint8_t canal;
    int16_t dernierEchantillon;
    EtatCapteur etat;
} CapteurHumidite;

static CapteurHumidite humidite =
{
    .canal = ADC_CANAL_HUMIDITE
};

void humidite_init(void)
{
    humidite.canal = ADC_CANAL_HUMIDITE;
    humidite.etat = CAPTEUR_OK;
}

float humidite_lire_pourcentage(void)
{
    uint16_t brut = adc_lire(humidite.canal);

    humidite.dernierEchantillon = (int16_t)brut;

    float pourcentage = (float)brut / 20.0f;

    if (pourcentage > 100.0f)
    {
        pourcentage = 100.0f;
    }

    return pourcentage;
}
