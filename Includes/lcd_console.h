/*
 * lcd_console.h
 *
 *  Created on: Jul 6, 2026
 *      Author: nxf82725
 */

#ifndef LCD_CONSOLE_H
#define LCD_CONSOLE_H

void lcd_console_init(void);
void lcd_console_clear(void);
void lcd_console_putchar(char c);
void lcd_console_print(const char *s);
char lcd_console_getchar(void);

#endif
