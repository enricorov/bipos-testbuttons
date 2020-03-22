/*
	Application template for Amazfit Bip BipOS
	(C) Maxim Volkov  2019 <Maxim.N.Volkov@ya.ru>
	
	Application template, header file

*/

#ifndef __APP_BUTTONS_H__
#define __APP_BUTTONS_H__
#define COLORS_COUNT	4

#include "helpers.h"



// template.c
void 	show_screen (void *return_screen);
void 	key_press_screen();
int 	dispatch_screen (void *param);
void 	refreshScreen();
void	testCallbackFunction(button_ button);

#endif