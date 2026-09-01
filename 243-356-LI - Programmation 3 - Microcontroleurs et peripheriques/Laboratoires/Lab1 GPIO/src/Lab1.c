/**
 ******************************************************************************
 * @file    Lab1.c
 * @brief   Lab 1 - GPIO bare metal STM32F103
 ******************************************************************************
 */

#define RCC_APB2ENR (*(volatile unsigned int *)0x40021018)

#define GPIOB_CRL   (*(volatile unsigned int *)0x40010C00)
#define GPIOB_CRH   (*(volatile unsigned int *)0x40010C04)
#define GPIOB_IDR   (*(volatile unsigned int *)0x40010C08)
#define GPIOB_ODR   (*(volatile unsigned int *)0x40010C0C)
#define GPIOB_BSRR  (*(volatile unsigned int *)0x40010C10)
#define GPIOB_BRR   (*(volatile unsigned int *)0x40010C14)


/* ---------------------------------------------------------------------
 * Function : GPIO_InitPin
 * Author   : Matisse Rhéaume-Viale & PROG3
 * Desc     : Configure une broche de GPIOB en entrée ou en sortie.
 * Notes    : mode = 1 : sortie push-pull 2 MHz
 *            mode = 0 : entrée flottante
 * --------------------------------------------------------------------- */
void GPIO_InitPin(unsigned char pin, unsigned char mode)
{
    unsigned int decalage;

    RCC_APB2ENR |= (1u << 3);    // Active l'horloge de GPIOB

    decalage = ((unsigned int)pin) * 4u;

    // Efface les 4 bits de configuration de la broche
    GPIOB_CRL &= ~(0xFu << decalage);

    if (mode == 1u)
    {
        // CNF = 00, MODE = 10
        // Sortie push-pull, 2 MHz
        GPIOB_CRL |= (0x2u << decalage);
    }
    else
    {
        // CNF = 01, MODE = 00
        // Entrée flottante
        GPIOB_CRL |= (0x4u << decalage);
    }
}


/* ---------------------------------------------------------------------
 * Function : GPIO_WritePin
 * Author   : Matisse Rhéaume-Viale & PROG3
 * Desc     : Écrit un état logique sur une broche de GPIOB.
 * Notes    : etat = 1 : niveau haut
 *            etat = 0 : niveau bas
 * --------------------------------------------------------------------- */
void GPIO_WritePin(unsigned char pin, unsigned char etat)
{
    if (etat == 1u)
    {
        GPIOB_BSRR = (1u << pin);
    }
    else
    {
        GPIOB_BRR = (1u << pin);
    }
}


/* ---------------------------------------------------------------------
 * Function : GPIO_ReadPin
 * Author   : Matisse Rhéaume-Viale & PROG3
 * Desc     : Lit l'état logique d'une broche de GPIOB.
 * Notes    : Retourne 0 ou 1.
 * --------------------------------------------------------------------- */
unsigned char GPIO_ReadPin(unsigned char pin)
{
    if ((GPIOB_IDR & (1u << pin)) != 0u)
    {
        return 1u;
    }
    else
    {
        return 0u;
    }
}


/* ---------------------------------------------------------------------
 * Function : Systeme_Delai
 * Author   : Matisse Rhéaume-Viale & PROG3
 * Desc     : Produit un délai logiciel approximatif en millisecondes.
 * Notes    : Boucle calibrée pour l'horloge HSI de 8 MHz.
 * --------------------------------------------------------------------- */
void Systeme_Delai(unsigned int delai)
{
    volatile unsigned int delaiOut = 0u;
    volatile unsigned int delaiIn;

    while (delaiOut < delai)
    {
        delaiOut++;

        delaiIn = 0u;

        while (delaiIn < 1000u)
        {
            delaiIn++;
        }
    }
}

int main(void)
{
    unsigned int delai = 300u;

    unsigned char boutonActuel;
    unsigned char boutonPrecedent = 1u;

    GPIO_InitPin(6u, 1u); // PB6 comme LED D1-1

    GPIO_InitPin(7u, 0u); // PB7 comme bouton S1-3

    GPIO_WritePin(6u, 1u); // LED d1-1 éteinte

    while (1)
    {
        boutonActuel = GPIO_ReadPin(7u); // Lire l'état du bouton S1-3

    /* ---------------------------------------------------------------------
    * Function : Systeme_Delai
    * Author   : Matisse Rhéaume-Viale & PROG3
    * Desc     : Produit un délai logiciel approximatif en millisecondes.
    * Notes    : Boucle calibrée pour l'horloge HSI de 8 MHz.
    * --------------------------------------------------------------------- */
        if ((boutonPrecedent == 1u) && (boutonActuel == 0u)) // Détection du front descendant
{
    Systeme_Delai(20u); // Délai anti-rebond de 20 ms

    if (GPIO_ReadPin(7u) == 0u) // Vérifie que le bouton est toujours appuyé
    {
        if (delai == 300u) // 300 ms -> 500 ms
        {
            delai = 500u;
        }
        else if (delai == 500u) // 500 ms -> 1000 ms
        {
            delai = 1000u;
        }
        else // 1000 ms -> 300 ms
        {
            delai = 300u;
        }
    }
}

        boutonPrecedent = boutonActuel;


        GPIO_WritePin(6u, 0u); // Allumer D1-1

        Systeme_Delai(delai); // Délai entre l'allumage et l'extinction de la LED

        GPIO_WritePin(6u, 1u); // Éteindre D1-1

        Systeme_Delai(delai); // Délai entre l'extinction et l'allumage de la LED
    }
}

