/*
 * cbm_port.c
 *
 *  Created on: Jul 6, 2026
 *      Author: nxf82725
 */

#include "lcd_console.h"

int platform_getchar(void)
{
    return lcd_console_getchar();
}

void platform_putchar(char c)
{
    lcd_console_putchar(c);
}



