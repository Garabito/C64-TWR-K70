/*
 * runtime_k70.c
 *
 *  Created on: Jul 6, 2026
 *      Author: nxf82725
 */

#include <stdint.h>

extern void lcd_console_putchar(char c);
extern char lcd_console_getchar(void);

void c64_putc(char c)
{
    lcd_console_putchar(c);
}

char c64_getc(void)
{
    return lcd_console_getchar();
}

void writestr(const char *s)
{
    while(*s)
    {
        c64_putc(*s++);
    }
}


void home_cursor(void)
{
}

int file_load(const char *name)
{
    return -1;
}

int file_save(const char *name)
{
    return -1;
}

void directory(void)
{
    writestr("DIRECTORY NOT SUPPORTED\r\n");
}

unsigned long settim(void)
{
    return 0;
}

void sleep_ms(unsigned ms)
{
    volatile unsigned i;

    while(ms--)
    {
        for(i=0;i<12000;i++)
        {
            __asm("nop");
        }
    }
}



