/*
	BipUI Buttons Demo
	v.0.1
	
*/

#include "buttons_test.h"

//	screen menu structure - each screen has its own
struct regmenu_ screen_data = {
	55,				  //	curr_screen - main screen number, value 0-255, for custom windows it is better to take from 50 and above
	1,				  //	swipe_scr - auxiliary screen number (usually 1)
	0,				  //	overlay - 0
	dispatch_screen,  //          - pointer to the handler of interaction (touch, swipe, long press)
	key_press_screen, //	        - handler of short button press
	refreshScreen,	//	        - timer callback function and..
	0,				  //	            ..the variable passed to it
	show_screen,	  //	        - function writing to video buffer and..
	0,				  //              ..the variable passed to it
	0				  //	        - handler of long button press
};

int main(int param0, char **argv)
{ //	here the variable argv is not defined
	show_screen((void *)param0);
}

// CALLBACK FUNCTIONS - functions associated to objects i.e. buttons or layers

void changeRefreshDelay(Layer_ *layer, Button_ button, short button_id)
{

	LayerParams_ params = layer->params;

	if (button_id == 2)
	{ // "plus" button

		if (params.refreshDelay <= 5000)
			params.refreshDelay += 200;
		else
			vibrate(1, 150, 0);
	}
	else if (button_id == 3) // "minus" button
	{
		if (params.refreshDelay >= 500)
			params.refreshDelay -= 200;
		else
			vibrate(1, 150, 0);
	}
}

void toggleAutorefresh(Layer_ *layer, short button_id)
{

	Button_ *thisButton = &layer->buttonArray[button_id];

	if (layer->params.refreshDelay == 0)
	{ // auto refresh is off

		_strcpy(layer->textBox.body, "Auto refresh ON");

		layer->params.refreshDelay = 2000; // turning it on
		thisButton->filling = COLOR_SH_GREEN;
		_strcpy(thisButton->label, "ON");
	}
	else
	{
		_strcpy(layer->textBox.body, "Auto refresh OFF");

		layer->params.refreshDelay = 0; // turning it off
		thisButton->filling = COLOR_SH_RED;
		_strcpy(thisButton->label, "OFF");
	}

	set_update_period(1, 20);		// triggering the update loop
}

void simpleInteractionCallbackFunction(Layer_ *layer, short button_id)
{
	Button_ button = layer->buttonArray[button_id];

	set_bg_color(getLongColour(button.filling));
	set_fg_color(getLongColour(button.text));

	fill_screen_bg();

	TextBox_ tempText = DEFAULT_TEXTBOX;

	tempText.background = button.filling;
	tempText.colour = button.text;
	char str[MAX_SIZE_TEXT_BOX];

	_sprintf(&str, "btn_id: %d\nlabel: %s\ntopLeft: (%d, %d)\nbtmRight:(%d, %d)\n\nRefreshing in 6s\n or swipe down",
			 button_id, button.label, button.topLeft.x, button.topLeft.y, button.bottomRight.x, button.bottomRight.y);

	_strcpy(tempText.body, str);

	drawTextBox(&tempText);

	repaint_screen_lines(0, VIDEO_Y);

	set_update_period(1, 6000); // schedule a refresh in 6s
}

Layer_ *layerButtonsConstructor(app_data_t *app_data)
{
	Layer_ *tempLayer = createLayer(); // allocating the space for the layer

	tempLayer->params.refreshDelay = 0; // initializing the params for the layer

	setLayerBackground(tempLayer, COLOR_SH_BLACK);

	short width = 81; // handy parameters for easier button creation
	short horizontalSeparation = 6;
	short verticalSeparation = 6;
	short height = 45;

	Point_ tempPointOne = BIPUI_BOTTOM_LEFT_POINT;
	Point_ tempPointTwo = BIPUI_BOTTOM_LEFT_POINT;

	tempPointOne.y -= height; // up by a height
	tempPointTwo.x += width;  // right by a width

	TextBox_ tempText = {

		.topLeft = BIPUI_TOP_LEFT_POINT,
		.bottomRight = BIPUI_BOTTOM_RIGHT_POINT,
		.body = "Auto refresh OFF",
		.colour = COLOR_SH_WHITE,
		.background = COLOR_SH_BLACK

	};

	setLayerTextBox(tempLayer, tempText);

	Button_ placeholderButton = DEFAULT_BUTTON_INSTANCE;
	placeholderButton.params.style = BUTTON_STYLE_ROUNDED_NOBORDER;
	placeholderButton.topLeft = tempPointOne;
	placeholderButton.bottomRight = tempPointTwo;
	_strcpy(placeholderButton.label, "HELP");
	placeholderButton.border = COLOR_SH_WHITE;
	placeholderButton.filling = COLOR_SH_BLACK;
	placeholderButton.text = COLOR_SH_WHITE;

	placeholderButton.callbackFunction = simpleInteractionCallbackFunction;

	addButtonToLayer(&placeholderButton, tempLayer); // button 0
	placeholderButton = moveInDirectionButton(&placeholderButton, RIGHT, horizontalSeparation);

	_strcpy(placeholderButton.label, "OFF");
	placeholderButton.filling = COLOR_SH_RED;
	placeholderButton.text = COLOR_SH_BLACK;
	placeholderButton.callbackFunction = toggleAutorefresh;

	addButtonToLayer(&placeholderButton, tempLayer); // button 1
	placeholderButton = moveInDirectionButton(&placeholderButton, UP, verticalSeparation);

	_strcpy(placeholderButton.label, "+");
	placeholderButton.filling = COLOR_SH_BLACK;
	placeholderButton.text = COLOR_SH_WHITE;
	placeholderButton.callbackFunction = changeRefreshDelay;

	addButtonToLayer(&placeholderButton, tempLayer); // button 2
	placeholderButton = moveInDirectionButton(&placeholderButton, LEFT, horizontalSeparation);

	_strcpy(placeholderButton.label, "-");
	placeholderButton.filling = COLOR_SH_BLACK;
	placeholderButton.text = COLOR_SH_WHITE;
	placeholderButton.callbackFunction = changeRefreshDelay;

	addButtonToLayer(&placeholderButton, tempLayer); // button 3

	return tempLayer;
}

void show_screen(void *param0)
{
	app_data_t **app_data_p = get_ptr_temp_buf_2(); //	pointer to a pointer to screen data
	app_data_t *app_data;							//	pointer to screen data

	Elf_proc_ *proc;

	// check the source at the procedure launch
	if ((param0 == *app_data_p) && get_var_menu_overlay())
	{ // return from the overlay screen (incoming call, notification, alarm, target, etc.)

		app_data = *app_data_p; //	the data pointer must be saved for the deletion
								//	release memory function reg_menu
		*app_data_p = NULL;		//	reset the pointer to pass it to the function reg_menu

		// 	create a new screen when the pointer temp_buf_2 is equal to 0 and the memory is not released
		reg_menu(&screen_data, 0); // 	menu_overlay=0

		*app_data_p = app_data;
	}
	else
	{ // if the function is started for the first time i.e. from the menu

		// create a screen (register in the system)
		reg_menu(&screen_data, 0);

		// allocate the necessary memory and place the data in it (the memory by the pointer stored at temp_buf_2 is released automatically by the function reg_menu of another screen)
		*app_data_p = (app_data_t *)pvPortMalloc(sizeof(app_data_t));
		app_data = *app_data_p; //	data pointer

		// clear the memory for data
		_memclr(app_data, sizeof(app_data_t));

		//	param0 value contains a pointer to the data of the running process structure Elf_proc_
		proc = param0;

		// remember the address of the pointer to the function you need to return to after finishing this screen
		if (param0 && proc->ret_f) //	if the return pointer is passed, then return to it
			app_data->ret_f = proc->elf_finish;
		else //	if not, to the watchface
			app_data->ret_f = show_watchface;

		begin(app_data);
	}

	caffeine(WEAK);
}

void begin(app_data_t *app_data)
{

	// Create layers here

	Layer_ *layerButtons = layerButtonsConstructor(app_data);

	setActiveLayerViewport(getCurrentViewport(app_data), layerButtons); // assigning the created layer to othe active slot of the viewport

	refreshLayer(getActiveLayer(app_data), 1); // drawing the layer
}

void key_press_screen()
{
	app_data_t **app_data_p = get_ptr_temp_buf_2(); //	pointer to a pointer to screen data
	app_data_t *app_data = *app_data_p;				//	pointer to screen data

	//destroyViewport(getCurrentViewport(app_data));

	// call the return function (usually this is the start menu), specify the address of the function of our application as a parameter
	show_menu_animate(app_data->ret_f, (unsigned int)show_screen, ANIMATE_RIGHT);
};

void refreshScreen()
{ // periodic
	// if necessary, you can use the screen data in this function
	app_data_t **app_data_p = get_ptr_temp_buf_2(); //	pointer to pointer to screen data
	app_data_t *app_data = *app_data_p;				//	pointer to screen data

	char str[20];

	if (getActiveLayer(app_data)->params.refreshDelay != 0)
	{	
		_sprintf(&str, "Period: %d", getActiveLayer(app_data)->params.refreshDelay);
		text_out_center(&str, 88, 30);
		set_update_period(1, (int) getActiveLayer(app_data)->params.refreshDelay);
	}

	refreshLayer(getActiveLayer(app_data), 1);
	vibrate(1, 50, 150);
}

int dispatch_screen(void *param)
{
	app_data_t **app_data_p = get_ptr_temp_buf_2(); //	pointer to a pointer to screen data
	app_data_t *app_data = *app_data_p;				//	pointer to screen data

	struct gesture_ *gest = param;
	int result = 0;

	/* if (getActiveOverlayValue(getActiveLayer(app_data)))
	{
		resetActiveOverlayValue(getActiveLayer(app_data));
		vibrate(3, 50, 150);
	}
	else
	{ */

	switch (gest->gesture)
	{
	case GESTURE_CLICK:
	{
		processTap(getActiveLayer(app_data), gest->touch_pos_x, gest->touch_pos_y);

		break;
	};
	case GESTURE_SWIPE_RIGHT:
	{ //	swipe to the right
		// moveToLayer()
		// show_menu_animate(app_data->ret_f, (unsigned int)show_screen, ANIMATE_RIGHT);
		break;
	};
	case GESTURE_SWIPE_LEFT:
	{ // swipe to the left
		// actions when swiping left
		break;
	};
	case GESTURE_SWIPE_UP:
	{ // swipe up
		// actions when swiping up
		break;
	};
	case GESTURE_SWIPE_DOWN:
	{ // swipe down

		//refreshLayer(getActiveLayer(app_data), 1); // manual refresh
		//set_update_period(0, 0);				   // removing scheduled refresh
		break;
	};
	default:
	{ // something went wrong ...

		break;
	};
	}
	//	}

	return result;
};