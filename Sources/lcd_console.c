/*
 * lcd_console.c
 *
 *  Created on: Jul 6, 2026
 *      Author: nxf82725
 */
#include "lcd_console.h"
#include <stdint.h>

#define LCD_W 480
#define LCD_H 272

static int cursor_x = 0;
static int cursor_y = 0;

static uint32_t *framebuffer =
    (uint32_t *)0x80000000;

void draw_char(
    int x,
    int y,
    char c,
    uint32_t fg,
    uint32_t bg);

void lcd_console_init(void)
{
    /*
     * SDRAM init
     * LCDC init
     * framebuffer setup
     */
}

void lcd_console_clear(void)
{
    for(int i=0;i<LCD_W*LCD_H;i++)
        framebuffer[i] = 0x000080;
}

 void lcd_console_putchar(char c)
{
    if(c=='\n')
    {
        cursor_x = 0;
        cursor_y++;
        return;
    }

    draw_char(
        cursor_x*8,
        cursor_y*8,
        c,
        0xFFFFFF,
        0x000080);

    cursor_x++;

    if(cursor_x>=60)
    {
        cursor_x=0;
        cursor_y++;
    }
}


void lcd_console_print(const char *s)
{
    while(*s)
        lcd_console_putchar(*s++);
}




