#include "sys.h"

/* WFI - Wait For Interrupt */
static inline void WFI_SET(void)
{
    __asm__ volatile ("wfi");
}

/* Disable interrupts (CPSID I) */
static inline void INTX_DISABLE(void)
{
    __asm__ volatile ("cpsid i");
}

/* Enable interrupts (CPSIE I) */
static inline void INTX_ENABLE(void)
{
    __asm__ volatile ("cpsie i");
}

/* Set Main Stack Pointer */
static inline void MSR_MSP(uint32_t addr)
{
    __asm__ volatile (
        "msr msp, %0\n"
        :
        : "r" (addr)
        : 
    );
}
