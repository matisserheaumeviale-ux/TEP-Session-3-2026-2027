#include <stdint.h>

#include "i2c.h"
#include "capteur.h"
#include "capteur_pression.h"

#define I2C_ADRESSE_PRESSION 0x76

typedef struct
{
    uint8_t adresse;
    int32_t dernierEchantillon;
    EtatCapteur etat;
} CapteurPression;

static CapteurPression pression =
{
    .adresse = I2C_ADRESSE_PRESSION
};

void pression_init(void)
{
    pression.adresse = I2C_ADRESSE_PRESSION;
    pression.etat = CAPTEUR_OK;
}

float pression_lire_hpa(void)
{
    uint32_t brut = i2c_lire_registre(pression.adresse);

    pression.dernierEchantillon = (int32_t)brut;

    return (float)brut / 100.0f;
}
