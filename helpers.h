// HELPER FUNCTIONS FOR BIPOS
//

#ifndef __HELPERS__
#define __HELPERS__

#include "libbip_EN.h"

#define MAX_NUM_BUTTONS 4
#define MAX_SIZE_BUTTON_LABEL 5

enum event_ble_ {
    UNKNOWN,
    PLAY,
    PAUSE,
    PLAYPAUSE,
    NEXT,
    PREVIOUS,
    VOLUMEUP,
    VOLUMEDOWN,
    FORWARD,
    REWIND
};

typedef struct button_
{
    unsigned short  a,      // upper left
                    b,      // upper right
                    c,      // lower left 
                    d;      // lower right

    char    label[MAX_SIZE_BUTTON_LABEL];

    short   border,         // color of button features
            filling,
            text;

    void*  callbackFunction;

} button_;

typedef struct database_ {

    button_ buttonArray[MAX_NUM_BUTTONS];   // all buttons
    unsigned short index;                   // current valid button, init=0

} database_;

void spawnButton(button_ *button, database_ *db);
void drawButton(button_ *button);
short addButtonToDB(button_ *button, database_ *db);
void createButton(button_ *button, short a, short b, short c, short d, char* label, short border,
                    short filling, short text, void* callbackFunction);

void spawnButton(button_ *button, database_ *db){

    if(!addButtonToDB(button, db)){

        drawButton(button);
    }
}

void createButton(button_ *button, short a, short b, short c, short d, char* label, short border,
                    short filling, short text, void* callbackFunction){     // populating the struct

                        button->a = a;
                        button->b = a;
                        button->c = a;
                        button->d = a;
                        _strcpy (button->label, label);
                        button->border = border;
                        button->filling = filling;
                        button->text = text;
                        button->callbackFunction = callbackFunction;
                    }

void drawButton(button_ *button){       // graphics of the button

    set_bg_color((long) button->filling);
    set_fg_color((long) button->border);

 //   fill_screen_bg();
    draw_filled_rect_bg(    (int) button->a,
                            (int) button->b,
                            (int) button->c,
                            (int) button->d);

/*     set_graph_callback_to_ram_1();      // moving the current drawings to the framebuffer?

    load_font();
    set_fg_color((long) button->text);

    text_out_center(    button->label,  // the text
                        (int) (button->a + button->b) / 2,  // median
                        (int) (button->a + button->c) / 2); */

}

short addButtonToDB(button_ *button, database_ *db){

    if(db->index >= MAX_NUM_BUTTONS){
        // db full
        //printError("DATABASE FULL");
        return 1;
    }
    else { // add button to db
        db->buttonArray[db->index] = *button;
        db->index++;
        return 0;
    }
}

void logthis(const char *message)    {

    log_printf(5, message);
    log_printf(5, "\r\n");
}

#endif