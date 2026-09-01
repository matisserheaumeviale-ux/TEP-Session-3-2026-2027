/**
 ******************************************************************************
 * @file    main.c
 * @brief   Lab 0 - Clignotement bare metal de la LED D1-1
 ******************************************************************************
 * Aucun HAL et aucune bibliothèque externe ne sont utilisés. Le STM32F103
 * démarre sur son oscillateur HSI de 8 MHz; GPIOC et SysTick sont configurés
 * directement par leurs registres mémoire.
 */

#include <stdint.h>

/* Registres RCC du STM32F103. */
#define RCC_BASE_ADDRESS          0x40021000UL
#define RCC_APB2ENR               (*(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x18UL))
#define RCC_APB2ENR_IOPCEN        (1UL << 4U)

/* Registres GPIOC du STM32F103. */
#define GPIOC_BASE_ADDRESS        0x40011000UL
#define GPIOC_CRH                 (*(volatile uint32_t *)(GPIOC_BASE_ADDRESS + 0x04UL))
#define GPIOC_BSRR                (*(volatile uint32_t *)(GPIOC_BASE_ADDRESS + 0x10UL))

/* SysTick Cortex-M3. */
#define SYST_CSR                  (*(volatile uint32_t *)0xE000E010UL)
#define SYST_RVR                  (*(volatile uint32_t *)0xE000E014UL)
#define SYST_CVR                  (*(volatile uint32_t *)0xE000E018UL)
#define SYST_CSR_ENABLE           (1UL << 0U)
#define SYST_CSR_TICKINT          (1UL << 1U)
#define SYST_CSR_CLKSOURCE        (1UL << 2U)

/* Mapping retenu pour le Lab 0 : D1-1 sur PC13, active à l'état haut. */
#define D1_1_PIN_NUMBER           13U
#define D1_1_PIN_MASK             (1UL << D1_1_PIN_NUMBER)
#define D1_1_ACTIVE_HIGH          1U
#define D1_1_BLINK_PERIOD_MS      500UL
#define HSI_CLOCK_HZ              8000000UL
#define SYSTICK_FREQUENCY_HZ      1000UL

#define M400_OFF                  0U
#define M400_ON                   1U

typedef struct
{
    uint32_t uiLastToggleMs;
    uint8_t ucState;
} M400_LED_t;

static volatile uint32_t g_uiNowMs = 0UL;

static void D1_1_vGPIOInit(void);
static void M400_vLEDSet(M400_LED_t *pxLED, uint8_t ucState);
static void M400_vLEDToggle(M400_LED_t *pxLED);
static void M400_vLEDBlinkTask(M400_LED_t *pxLED,
                               uint32_t uiNowMs,
                               uint32_t uiPeriodMs,
                               uint8_t ucEnabled);
static void SysTick_vInit(void);
static void CPU_vWaitForInterrupt(void);

/* Appelée par le startup avant main(). Le HSI 8 MHz de reset est conservé. */
void SystemInit(void)
{
}

/* Cette fonction remplace l'alias faible du startup. */
void SysTick_Handler(void)
{
    g_uiNowMs++;
}

int main(void)
{
    M400_LED_t xD1_1 = {0UL, M400_OFF};

    D1_1_vGPIOInit();
    M400_vLEDSet(&xD1_1, M400_OFF);
    SysTick_vInit();

    for (;;)
    {
        M400_vLEDBlinkTask(&xD1_1,
                           g_uiNowMs,
                           D1_1_BLINK_PERIOD_MS,
                           1U);
        CPU_vWaitForInterrupt();
    }
}

static void D1_1_vGPIOInit(void)
{
    uint32_t uiConfiguration;

    RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;
    (void)RCC_APB2ENR;

    /* Précharge OFF avant de configurer PC13 en sortie. */
    GPIOC_BSRR = (D1_1_ACTIVE_HIGH != 0U)
                   ? (D1_1_PIN_MASK << 16U)
                   : D1_1_PIN_MASK;

    /* PC13 est dans CRH, bits 23:20. CNF13=00, MODE13=10 (2 MHz). */
    uiConfiguration = GPIOC_CRH;
    uiConfiguration &= ~(0xFUL << 20U);
    uiConfiguration |= (0x2UL << 20U);
    GPIOC_CRH = uiConfiguration;
}

static void M400_vLEDSet(M400_LED_t *pxLED, uint8_t ucState)
{
    uint8_t ucTurnOn;
    uint8_t ucDriveHigh;

    if (pxLED == (M400_LED_t *)0)
    {
        return;
    }

    ucTurnOn = (ucState != M400_OFF) ? 1U : 0U;
    ucDriveHigh = (ucTurnOn == D1_1_ACTIVE_HIGH) ? 1U : 0U;

    /* BSRR est atomique : SET dans 15:0, RESET dans 31:16. */
    GPIOC_BSRR = (ucDriveHigh != 0U)
                   ? D1_1_PIN_MASK
                   : (D1_1_PIN_MASK << 16U);
    pxLED->ucState = ucTurnOn;
}

static void M400_vLEDToggle(M400_LED_t *pxLED)
{
    if (pxLED == (M400_LED_t *)0)
    {
        return;
    }

    M400_vLEDSet(pxLED,
                 (pxLED->ucState == M400_OFF) ? M400_ON : M400_OFF);
}

static void M400_vLEDBlinkTask(M400_LED_t *pxLED,
                               uint32_t uiNowMs,
                               uint32_t uiPeriodMs,
                               uint8_t ucEnabled)
{
    if (pxLED == (M400_LED_t *)0)
    {
        return;
    }

    if (ucEnabled == 0U)
    {
        M400_vLEDSet(pxLED, M400_OFF);
        pxLED->uiLastToggleMs = uiNowMs;
        return;
    }

    if ((uint32_t)(uiNowMs - pxLED->uiLastToggleMs) >= uiPeriodMs)
    {
        pxLED->uiLastToggleMs = uiNowMs;
        M400_vLEDToggle(pxLED);
    }
}

static void SysTick_vInit(void)
{
    SYST_CSR = 0UL;
    SYST_RVR = (HSI_CLOCK_HZ / SYSTICK_FREQUENCY_HZ) - 1UL;
    SYST_CVR = 0UL;
    SYST_CSR = SYST_CSR_CLKSOURCE | SYST_CSR_TICKINT | SYST_CSR_ENABLE;
}

static void CPU_vWaitForInterrupt(void)
{
    __asm volatile ("wfi");
}
