/*
 * keyboard.c
 *
 *  Created on: Jul 6, 2026
 *      Author: nxf82725
 */

#include "keyboard.h"
#include "MK70F12.h"

#define UART_BAUD 115200U
#define BUS_CLOCK 60000000U

void keyboard_init(void)
{
    /* Habilitar reloj UART5 */
    SIM->SCGC1 |= SIM_SCGC1_UART5_MASK;

    /* Habilitar puerto para pines UART */
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    /* PTE8 = RX */
    /* PTE9 = TX */

    PORTE->PCR[8] =
        PORT_PCR_MUX(3);

    PORTE->PCR[9] =
        PORT_PCR_MUX(3);

    UART5->C2 = 0;

    uint16_t sbr =
        (BUS_CLOCK / (16 * UART_BAUD));

    UART5->BDH =
        (sbr >> 8) & UART_BDH_SBR_MASK;

    UART5->BDL =
        (uint8_t)sbr;

    UART5->C1 = 0;

    UART5->C2 =
        UART_C2_RE_MASK |
        UART_C2_TE_MASK;
}


char keyboard_getchar(void)
{
    while(!(UART5->S1 & UART_S1_RDRF_MASK))
    {
    }

    return (char)UART5->D;
}

int keyboard_kbhit(void)
{
    if(UART5->S1 & UART_S1_RDRF_MASK)
    {
        return 1;
    }

    return 0;
}

