/*
 * keyboard.h
 *
 *  Created on: Jul 6, 2026
 *      Author: nxf82725
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
char keyboard_getchar(void);
int keyboard_kbhit(void);

#endif


