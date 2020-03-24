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
void 	testCallbackFunction(Layer_ *layer, button_ button, short button_id);
Layer_ *layerSplashConstructor(app_data_t *app_data);
Layer_ *layerButtonsConstructor(app_data_t *app_data);
void 	splashCallbackFunction(Viewport_ *vp);

#endif