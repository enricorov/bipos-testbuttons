/*
	Application template for Amazfit Bip BipOS
	(C) Maxim Volkov  2019 <Maxim.N.Volkov@ya.ru>
	
	Application template loader for the BipOS
	
*/

//#include "helpers.h"
#include "buttons_test.h"

//	screen menu structure - each screen has its own
struct regmenu_ screen_data = {
						55,							//	main screen number, value 0-255, for custom windows it is better to take from 50 and above
						1,							//	auxiliary screen number (usually 1)
						0,							//	0
						dispatch_screen,			//	pointer to the function handling touch, swipe, long press
						key_press_screen, 			//	pointer to the function handling pressing the button
						screen_job,					//	pointer to the callback function of the timer  
						0,							//	0
						show_screen,				//	pointer to the screen display function
						0,							//	
						0							//	long press of the button
					};
int main(int param0, char** argv){	//	here the variable argv is not defined
	show_screen((void*) param0);
}

void testCallbackFunction(){

	set_bg_color(COLOR_RED);
	fill_screen_bg();
	repaint_screen_lines(0, VIDEO_Y);

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

	button_ sampleButton;
	createButton(	&sampleButton, 
					50, 50,
					80, 80,
					"",
					COLOR_SH_WHITE,
					COLOR_SH_PURPLE,
					0,
					testCallbackFunction);

	setLayerBackground(getCurrentLayer(app_data), COLOR_SH_BLACK);
	

	spawnButton(&sampleButton, getCurrentLayer(app_data));

	createButton(	&sampleButton, 
					100, 100,
					120, 120,
					"",
					COLOR_SH_WHITE,
					COLOR_SH_PURPLE,
					0,
					0);

	spawnButton(&sampleButton, getCurrentLayer(app_data));

}	

caffeine();
//set_update_period(1, 500);



}

void key_press_screen(){
	app_data_t** 	app_data_p = get_ptr_temp_buf_2(); 	//	pointer to a pointer to screen data 
	app_data_t *	app_data = *app_data_p;				//	pointer to screen data

	// call the return function (usually this is the start menu), specify the address of the function of our application as a parameter
	show_menu_animate(app_data->ret_f, (unsigned int)show_screen, ANIMATE_RIGHT);	
};

void screen_job(){		// periodic
	// if necessary, you can use the screen data in this function
	app_data_t** 	app_data_p = get_ptr_temp_buf_2(); 	//	pointer to pointer to screen data  
	app_data_t *	app_data = *app_data_p;				//	pointer to screen data


	refreshLayer(getCurrentLayer(app_data));
//	set_update_period(1, 500);
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
			// usually this is the exit from the application
			show_menu_animate(app_data->ret_f, (unsigned int)show_screen, ANIMATE_RIGHT);	
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