#include <stdio.h>

#include "adc.h"
#include "i2c.h"
#include "capteur_temperature.h"
#include "capteur_humidite.h"
#include "capteur_pression.h"

int main(void)
{
    adc_init();
    i2c_init();

    temperature_init();
    humidite_init();
    pression_init();

    for (int cycle = 0; cycle < 5; cycle++)
    {
        float tempC = temperature_lire_celsius();
        float humPct = humidite_lire_pourcentage();
        float presHpa = pression_lire_hpa();

        printf("Cycle %d :\n", cycle);
        printf(" Temperature = %.1f C\n", tempC);
        printf(" Humidite = %.1f %%\n", humPct);
        printf(" Pression = %.1f hPa\n", presHpa);
    }

    return 0;
}
