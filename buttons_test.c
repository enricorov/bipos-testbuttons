/*
	Application template for Amazfit Bip BipOS
	(C) Maxim Volkov  2019 <Maxim.N.Volkov@ya.ru>
	
	Application template loader for the BipOS
	
*/

//#include "helpers.h"
#include "buttons_test.h"

//	screen menu structure - each screen has its own
struct regmenu_ screen_data = {
						55,							    //	curr_screen - main screen number, value 0-255, for custom windows it is better to take from 50 and above
						1,							    //	swipe_scr - auxiliary screen number (usually 1)
						0,							    //	overlay - 0
						dispatch_screen,		//          - pointer to the handler of interaction (touch, swipe, long press)
						key_press_screen, 	//	        - handler of short button press
						refreshScreen,					//	        - timer callback function and..
						0,							    //	            ..the variable passed to it
						show_screen,				//	        - function writing to video buffer and..
						0,							    //              ..the variable passed to it
						0							      //	        - handler of long button press
					};
					
int main(int param0, char** argv){	//	here the variable argv is not defined
	show_screen((void*) param0);
}

void testCallbackFunction(button_ button){

	set_bg_color(getLongColour(button.filling));
	set_fg_color(getLongColour(button.text));

	fill_screen_bg();

	text_out_center(button.label, VIDEO_Y/2, VIDEO_X/2);

	repaint_screen_lines(0, VIDEO_Y);
	
	set_update_period(1, 1000);			// request a refresh

}

void show_screen (void *param0){
app_data_t** 	app_data_p = get_ptr_temp_buf_2(); 	//	pointer to a pointer to screen data
app_data_t *	app_data;					//	pointer to screen data

Elf_proc_* proc;

// check the source at the procedure launch
if ( (param0 == *app_data_p) && get_var_menu_overlay()){ // return from the overlay screen (incoming call, notification, alarm, target, etc.)

	app_data = *app_data_p;					//	the data pointer must be saved for the deletion 
											//	release memory function reg_menu
	*app_data_p = NULL;						//	reset the pointer to pass it to the function reg_menu		

	// 	create a new screen when the pointer temp_buf_2 is equal to 0 and the memory is not released		
	reg_menu(&screen_data, 0); 				// 	menu_overlay=0
	
	*app_data_p = app_data;						//	restore the data pointer after the reg_menu function
	
	//   here we perform actions when returning from the overlay screen: restore data, etc.
	
	
} else { // if the function is started for the first time i.e. from the menu 

	// create a screen (register in the system) 
	reg_menu(&screen_data, 0);

	// allocate the necessary memory and place the data in it (the memory by the pointer stored at temp_buf_2 is released automatically by the function reg_menu of another screen)
	*app_data_p = (app_data_t *)pvPortMalloc(sizeof(app_data_t));
	app_data = *app_data_p;		//	data pointer
	
	// clear the memory for data
	_memclr(app_data, sizeof(app_data_t));
	
	//	param0 value contains a pointer to the data of the running process structure Elf_proc_
	proc = param0;
	
	// remember the address of the pointer to the function you need to return to after finishing this screen
	if ( param0 && proc->ret_f ) 			//	if the return pointer is passed, then return to it
		app_data->ret_f = proc->elf_finish;
	else					//	if not, to the watchface
		app_data->ret_f = show_watchface;
	
	
	// setup part, the first graphics are created here

	setLayerBackground(getCurrentLayer(app_data), COLOR_SH_BLACK);

	button_ placeholderButton;
	initButton(	&placeholderButton, 			// initial button on the top left
					4, 4,
					82, 50,	
					"HELLO",
					COLOR_SH_WHITE,
					COLOR_SH_BLUE,
					COLOR_SH_YELLOW,
					testCallbackFunction);

	spawnButton(&placeholderButton, getCurrentLayer(app_data));
	placeholderButton = moveInDirectionButton(&placeholderButton, RIGHT, 4);	// top right
	
	_strcpy(placeholderButton.label, "WORLD");
	placeholderButton.filling = COLOR_SH_RED;
	placeholderButton.text = COLOR_SH_BLACK;

	spawnButton(&placeholderButton, getCurrentLayer(app_data));
	placeholderButton = moveInDirectionButton(&placeholderButton, DOWN, 4);		// mid right
	
	_strcpy(placeholderButton.label, "ASDF");
	placeholderButton.filling = COLOR_SH_AQUA;
	placeholderButton.text = COLOR_SH_PURPLE;

	spawnButton(&placeholderButton, getCurrentLayer(app_data));
	placeholderButton = moveInDirectionButton(&placeholderButton, LEFT, 4);		// mid left
	
	_strcpy(placeholderButton.label, "DONG");
	placeholderButton.filling = COLOR_SH_GREEN;
	placeholderButton.text = COLOR_SH_BLACK;

	spawnButton(&placeholderButton, getCurrentLayer(app_data));
	placeholderButton = moveInDirectionButton(&placeholderButton, DOWN, 4);		// low left
	
	_strcpy(placeholderButton.label, "FAM");
	placeholderButton.filling = COLOR_SH_PURPLE;
	placeholderButton.text = COLOR_SH_WHITE;

	spawnButton(&placeholderButton, getCurrentLayer(app_data));
	placeholderButton = moveInDirectionButton(&placeholderButton, RIGHT, 4);		// low right
	
	_strcpy(placeholderButton.label, "BURP");
	placeholderButton.filling = COLOR_SH_YELLOW;
	placeholderButton.text = COLOR_SH_BLACK;

	spawnButton(&placeholderButton, getCurrentLayer(app_data));

	refreshLayer(getCurrentLayer(app_data));						// redraw all the layer

}	

caffeine(WEAK);

}

void key_press_screen(){
	app_data_t** 	app_data_p = get_ptr_temp_buf_2(); 	//	pointer to a pointer to screen data 
	app_data_t *	app_data = *app_data_p;				//	pointer to screen data

	// call the return function (usually this is the start menu), specify the address of the function of our application as a parameter
	show_menu_animate(app_data->ret_f, (unsigned int)show_screen, ANIMATE_RIGHT);	
};

void refreshScreen(){		// periodic
	// if necessary, you can use the screen data in this function
	app_data_t** 	app_data_p = get_ptr_temp_buf_2(); 	//	pointer to pointer to screen data  
	app_data_t *	app_data = *app_data_p;				//	pointer to screen data


	refreshLayer(getCurrentLayer(app_data));
	set_update_period(0, 0);		// refreshed, turning off timer refresh
}

int dispatch_screen (void *param){
	app_data_t** 	app_data_p = get_ptr_temp_buf_2(); 	//	pointer to a pointer to screen data 
	app_data_t *	app_data = *app_data_p;				//	pointer to screen data

	// in case of rendering the interface, update (transfer to video memory) the screen

	struct gesture_ *gest = param;
	int result = 0;


	switch (gest->gesture){
		case GESTURE_CLICK: {			

				processTap(getCurrentLayer(app_data), gest->touch_pos_x, gest->touch_pos_y);

				break;
			};
			case GESTURE_SWIPE_RIGHT: {	//	swipe to the right
				// exit with swipe disabled
				// show_menu_animate(app_data->ret_f, (unsigned int)show_screen, ANIMATE_RIGHT);	
				break;
			};
			case GESTURE_SWIPE_LEFT: {	// swipe to the left
				// actions when swiping left	
				break;
			};
			case GESTURE_SWIPE_UP: {	// swipe up
				// actions when swiping up
				break;
			};
			case GESTURE_SWIPE_DOWN: {	// swipe down
				// actions when swiping down
				break;
			};		
			default:{	// something went wrong ...
				
				break;
			};		
			
		}
	
	return result;
};