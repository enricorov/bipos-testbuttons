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
    unsigned short  a,      // x1   upper left
                    b,      // y1
                    c,      // x2   lower right
                    d;      // y2

    char    label[MAX_SIZE_BUTTON_LABEL];

    short   border,         // color of button features
            filling,
            text;

    void*  callbackFunction;

} button_;

typedef struct Gcontext_ {

    button_ buttonArray[MAX_NUM_BUTTONS];   // all buttons
    unsigned short index;                   // current valid button, init=0
    short   backgroundColour;               // background for the current context

} Gcontext_;

void spawnButton(button_ *button, Gcontext_ *ctx);
void drawButton(button_ *button);
short addButtonToContext(button_ *button, Gcontext_ *ctx);
void createButton(button_ *button, short a, short b, short c, short d, char* label, short border,
                    short filling, short text, void* callbackFunction);
long getLongColour(short colour);     // returns long from short versions
void caffeine(void);                  // display never turns off
void setContextBackground(Gcontext_ *ctx, short colour);

// DEFINITIONS ---------------------------------------------------

void caffeine(void){

  set_display_state_value (8, 1);
  set_display_state_value (4, 1);
  set_display_state_value (2, 1);

}

void spawnButton(button_ *button, Gcontext_ *ctx){

    if(!addButtonToContext(button, ctx)){

        drawButton(button);
    }
}

void createButton(button_ *button, short a, short b, short c, short d, char* label, short border,
                    short filling, short text, void* callbackFunction){     // populating the struct

                        button->a = a;
                        button->b = b;
                        button->c = c;
                        button->d = d;
                        _strcpy (button->label, label);
                        button->border = border;
                        button->filling = filling;
                        button->text = text;
                        button->callbackFunction = callbackFunction;
                    }

void drawButton(button_ *button){       // graphics of the button

    set_bg_color(getLongColour(button->filling));
    set_fg_color(getLongColour(button->border ));

 //   fill_screen_bg();
    draw_filled_rect_bg(    button->a,
                            button->b,
                            button->c,
                            button->d);
    
    draw_rect(              button->a,
                            button->b,
                            button->c,
                            button->d);

    set_graph_callback_to_ram_1();      // moving the current drawings to the framebuffer?


    load_font();
    set_fg_color(getLongColour(button->text));

    text_out_center(    button->label,  // the text
                        (int) (button->a + button->c) / 2,  // median
                        (int) (button->b + button->d) / 2); 
    repaint_screen_lines(0, 176);
}

long getLongColour(short colour) {

    switch(colour) {

        case COLOR_SH_AQUA:
            return COLOR_AQUA;
        case COLOR_SH_BLACK:
            return COLOR_BLACK;
        case COLOR_SH_BLUE:
            return COLOR_BLUE;
        case COLOR_SH_GREEN:
            return COLOR_GREEN;
        case COLOR_SH_MASK:
            return COLOR_MASK;
        case COLOR_SH_PURPLE:
            return COLOR_PURPLE;
        case COLOR_SH_RED:
            return COLOR_RED;
        case COLOR_SH_WHITE:
            return COLOR_WHITE;
        case COLOR_SH_YELLOW:
            return COLOR_YELLOW;
        default:{
            // should never get here..
            };
    };
    return 0;

}

void setContextBackground(Gcontext_ *ctx, short colour){

    ctx->backgroundColour = colour;

}

short addButtonToContext(button_ *button, Gcontext_ *ctx){

    if(ctx->index >= MAX_NUM_BUTTONS){
        // ctx full
        //printError("DATABASE FULL");
        return 1;
    }
    else { // add button to ctx
        ctx->buttonArray[ctx->index] = *button;
        ctx->index++;
        return 0;
    }
}

void logthis(const char *message)    {

    log_printf(5, message);
    log_printf(5, "\r\n");
}

#endif