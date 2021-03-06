/*
	BipUI Buttons Demo
	v.0.1
	
*/

#include "buttons_test.h"

#ifdef __SIMULATION__
	#include <stdio.h>
	#include <string.h>
#endif

//	screen menu structure - each screen has its own
struct regmenu_ screen_data = {
	55,				  //	curr_screen - main screen number, value 0-255, for custom windows it is better to take from 50 and above
	1,				  //	swipe_scr - auxiliary screen number (usually 1)
	0,				  //	overlay - 0
	interactionHandler,  //          - pointer to the handler of interaction (touch, swipe, long press)
	key_press_screen, //	        - handler of short button press
	refreshScreen,	//	        - timer callback function and..
	0,				  //	            ..the variable passed to it
	show_screen,	  //	        - function writing to video buffer and..
	0,				  //              ..the variable passed to it
	0				  //	        - handler of long button press
};

#ifdef __SIMULATION__
int main_app(int param0)
	{
#else
int main(int param0, char **argv)
{ //	here the variable argv is not defined
#endif
	show_screen((void *)param0);
}

// CALLBACK FUNCTIONS - functions associated to objects i.e. buttons or layers

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

	#ifdef __SIMULATION__
		sprintf((char *)&str, "btn_id: %d\nlabel: %s\ntopLeft: (%d, %d)\nbtmRight:(%d, %d)\n\nRefreshing in 6s\n or swipe down",
				button_id, button.label, button.topLeft.x, button.topLeft.y, button.bottomRight.x, button.bottomRight.y);

		strcpy(tempText.body, str);
	#else
		_sprintf(&str, "btn_id: %d\nlabel: %s\ntopLeft: (%d, %d)\nbtmRight:(%d, %d)\n\nRefreshing in 6s\n or swipe down",
			button_id, button.label, button.topLeft.x, button.topLeft.y, button.bottomRight.x, button.bottomRight.y);

		_strcpy(tempText.body, str);
	#endif

	drawTextBox(&tempText);

	//repaint_screen_lines(0, VIDEO_Y);
	repaint_screen();

	set_update_period(1, 6000); // schedule a refresh in 6s
}

// CONSTRUCTORS - Defining elements, put the messy intializations here

Layer_ *layerButtonsConstructor(app_data_t *app_data)
{

	short width = 82; // handy parameters for easier button creation
	short horizontalSeparation = 6;
	short verticalSeparation = 6;
	short height = 45;

	Point_ tempPointOne = BIPUI_BOTTOM_LEFT_POINT;
	Point_ tempPointTwo = BIPUI_BOTTOM_LEFT_POINT;

	tempPointOne.y -= height;
	tempPointTwo.x += width;

	
	Layer_ *tempLayer = createLayer(); // allocating the space for the layer
	//setActiveLayerViewport(getCurrentViewport(app_data), tempLayer); // assigning this layer
	// to the active slot of this viewport

	setLayerBackground(tempLayer, COLOR_SH_BLACK);

	TextBox_ tempText;

	tempText.topLeft = BIPUI_TOP_LEFT_POINT;
	tempText.bottomRight = BIPUI_BOTTOM_RIGHT_POINT;
	_strcpy(tempText.body, "Tap any button");
	tempText.colour = COLOR_SH_WHITE;
	tempText.background = COLOR_SH_BLACK;

	setLayerTextBox(tempLayer, tempText);

	Button_ placeholderButton;
	initButton(&placeholderButton, // initial button on the bottom left
			   tempPointOne,
			   tempPointTwo,
			   "HELLO",
			   COLOR_SH_WHITE,
			   COLOR_SH_BLUE,
			   COLOR_SH_YELLOW,
			   simpleInteractionCallbackFunction);

	addButtonToLayer(&placeholderButton, tempLayer);
	placeholderButton = moveInDirectionButton(&placeholderButton, RIGHT, horizontalSeparation); // top right

	_strcpy(placeholderButton.label, "WORLD");
	placeholderButton.filling = COLOR_SH_RED;
	placeholderButton.text = COLOR_SH_BLACK;

	addButtonToLayer(&placeholderButton, tempLayer);
	placeholderButton = moveInDirectionButton(&placeholderButton, UP, verticalSeparation); // mid right

	_strcpy(placeholderButton.label, "ASDF");
	placeholderButton.filling = COLOR_SH_AQUA;
	placeholderButton.text = COLOR_SH_BLUE;

	addButtonToLayer(&placeholderButton, tempLayer);
	placeholderButton = moveInDirectionButton(&placeholderButton, LEFT, horizontalSeparation); // mid left

	_strcpy(placeholderButton.label, "DONG");
	placeholderButton.filling = COLOR_SH_GREEN;
	placeholderButton.text = COLOR_SH_BLACK;

	addButtonToLayer(&placeholderButton, tempLayer);
	placeholderButton = moveInDirectionButton(&placeholderButton, UP, verticalSeparation); // low left

	_strcpy(placeholderButton.label, "DEAD");
	placeholderButton.filling = COLOR_SH_PURPLE;
	placeholderButton.text = COLOR_SH_WHITE;

	addButtonToLayer(&placeholderButton, tempLayer);
	placeholderButton = moveInDirectionButton(&placeholderButton, RIGHT, horizontalSeparation); // low right

	_strcpy(placeholderButton.label, "BEEF");
	placeholderButton.filling = COLOR_SH_YELLOW;
	placeholderButton.text = COLOR_SH_BLUE;

	addButtonToLayer(&placeholderButton, tempLayer);

	return tempLayer;
}

// Utility functions

void show_screen(void *param0)
{
	#ifdef __SIMULATION__
		app_data_t *app_data = get_app_data_ptr();
		app_data_t **app_data_p = &app_data;
	#else
		app_data_t **app_data_p = get_ptr_temp_buf_2(); //	pointer to a pointer to screen data
		app_data_t *app_data;							//	pointer to screen data
	#endif

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

		#ifdef __SIMULATION__
			set_app_data_ptr(app_data);
		#endif

		begin(app_data);
	}

	caffeine(WEAK);
}

void begin(app_data_t *app_data)
{

	// Create layers here

	Layer_ *layerButtons = layerButtonsConstructor(app_data);
	
	setActiveLayerViewport(getCurrentViewport(app_data), layerButtons);  // assigning the created layer to othe active slot of the viewport

	refreshLayer(getActiveLayer(app_data), 1);	// drawing the layer

}

void key_press_screen()
{
	#ifdef __SIMULATION__
		app_data_t *app_data = get_app_data_ptr();
		app_data_t **app_data_p = &app_data;
	#else
		app_data_t **app_data_p = get_ptr_temp_buf_2(); //	pointer to a pointer to screen data
		app_data_t *app_data = *app_data_p;							//	pointer to screen data
	#endif
	//destroyViewport(getCurrentViewport(app_data));

	// call the return function (usually this is the start menu), specify the address of the function of our application as a parameter
	show_menu_animate(app_data->ret_f, (unsigned int)show_screen, ANIMATE_RIGHT);
};

void refreshScreen()
{ // triggered by set_update_period

	#ifdef __SIMULATION__
		app_data_t *app_data = get_app_data_ptr();
		app_data_t **app_data_p = &app_data;
	#else
		app_data_t **app_data_p = get_ptr_temp_buf_2(); //	pointer to a pointer to screen data
		app_data_t *app_data = *app_data_p;							//	pointer to screen data
	#endif
	refreshLayer(getActiveLayer(app_data), 1);
	vibrate(2, 50, 150);

}

int interactionHandler(void *param)
{
	#ifdef __SIMULATION__
		app_data_t *app_data = get_app_data_ptr();
		app_data_t **app_data_p = &app_data;
	#else
		app_data_t **app_data_p = get_ptr_temp_buf_2(); //	pointer to a pointer to screen data
		app_data_t *app_data = *app_data_p;							//	pointer to screen data
	#endif
	
	struct gesture_ *gest = param;
	int result = 0;

	switch (gest->gesture)
	{
	case GESTURE_CLICK:
	{
		vibrate(1, 50, 0);
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

		refreshLayer(getActiveLayer(app_data), 1); // manual refresh
		set_update_period(0, 0);				   // removing scheduled refresh
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