// HELPER FUNCTIONS FOR BIPOS
//

#ifndef __HELPERS__
#define __HELPERS__

#include "libbip_EN.h"
//#include "buttons_test.h"

#define MAX_NUM_BUTTONS 8
#define MAX_SIZE_BUTTON_LABEL 20
#define MAX_SIZE_TEXT_BOX 100

#define MAX_NUM_LAYERS 2

// UI

#define DEFAULT_BORDER_THICKNESS 4      // distance of button edge to screen

typedef struct Point_ {

    short   x,
            y;
} Point_;

typedef enum Caffeine_t {

    WEAK,   // 0 - screen backlight will go off
    STRONG  // 1 - backlight always on

} Caffeine_t;

typedef enum Way_ {

    UP,
    DOWN,
    LEFT,
    RIGHT

} Way_;

typedef struct button_
{
    /* unsigned short  a,      // x1   upper left
                    b,      // y1
                    c,      // x2   lower right
                    d;      // y2
 */
    Point_  topLeft,
            bottomRight;

    char    label[MAX_SIZE_BUTTON_LABEL];

    short   border,         // color of button features
            filling,
            text;

    void (*callbackFunction)();
} button_;

typedef struct TextBox_ {

    unsigned short  x1,      // x1   upper left
                    y1,      // y1
                    x2,      // x2   lower right
                    y2;      // y2

    char    body[MAX_SIZE_TEXT_BOX];

    short   colour;

} TextBox_;

typedef struct Layer_ {

    button_ buttonArray[MAX_NUM_BUTTONS];   // all buttons
    unsigned short index;                   // current valid button, init=0

    short   backgroundColour;               // background for the current Layer
    short   visible;                        // is the layer visible?
    TextBox_ textBox;               // textbox for general usage   

    void (*callbackFunction)();
} Layer_;

typedef struct Window_ {

    Layer_ layerArray[MAX_NUM_LAYERS];
    short index;

} Window_;

typedef struct app_data_t {
			void* 	ret_f;					//	the address of the return function
			Layer_	mainLayer;
            Window_ mainWindow;
} app_data_t;

// CONSTANTS ----------------------

const static Point_ UI_TOP_LEFT_POINT = {

    .x = DEFAULT_BORDER_THICKNESS,
    .y = DEFAULT_BORDER_THICKNESS
};

const static Point_ UI_BOTTOM_LEFT_POINT = {

    .x = DEFAULT_BORDER_THICKNESS,
    .y = VIDEO_X - DEFAULT_BORDER_THICKNESS
};

// PROTOTYPES --------------------------

void initButton(button_ *button, Point_ topLeft, Point_ bottomRight, // initialize button with these parameters
                    char* label, short border, short filling, short text, void* callbackFunction);
void    spawnButton(button_ *button, Layer_ *layer);       // adds button to layer and draws it - note: graphics are shown only after calling refresh_screen_lines()
void    drawButton(button_ *button);                       // draws a button only
short   addButtonToLayer(button_ *button, Layer_ *layer); // adds button to layer

long    getLongColour(short colour);     // returns long from short versions
void    caffeine(Caffeine_t coffee);     // set display backlight
void    setLayerBackground(Layer_ *layer, short colour);
Layer_ *createLayer(void);
void destroyLayer(Layer_ * layer); 
Layer_ *getCurrentLayer(Window_ * window);  // returns layer currently in use
Layer_ *getTopLayer(app_data_t *app_data);  // returns topmost layer
void processTap(Window_ *window, int x, int y);                 // iterates over a layer for the button corresponding to a tap
button_ moveInDirectionButton(button_ *button, Way_ dir, short offset); // given a button, it changes its parameters to move it.
button_ mirrorInDirectionButton(button_ *button, Way_ dir);             // mirrors a button with respect to one of the four axes
short   findHighestOpaqueLayer(Window_ *window);                           // returns the highest indexed layer with bg != COLOR_SH_MASK
Window_ *getCurrentWindow(app_data_t *app_data);
short   getCurrentLayerIndex(Window_ *window);
void    initializeLayer(Layer_ *layer);        // setting layer params to default
void    setLayerTextBox(Layer_ * layer, char *string);          // setting text box for a given layer
short   addLayerToWindow(Layer_ * layer, Window_ *window);            
void    spawnLayer(Layer_ * layer, Window_ *window);   
void    initializeTextBox(TextBox_ *textbox, short x1, short y1, short x2, short y2, short colour);
void    refreshWindow(Window_ *window);
void    drawTextBox(TextBox_ *textbox);
void refreshLayer(Layer_ *layer, short repaint);
void    initializeWindow(Window_ *window);

// DEFINITIONS ---------------------------------------------------

void initializeWindow(Window_ *window) {

    window->index = -1;
}

void drawTextBox(TextBox_ *box){

    set_fg_color(getLongColour(box->colour));

    text_out_center(    box->body,  // the text
                        (int) (box->x1 + box->x2) / 2,  // median
                        (int) box->y1);

}

void refreshWindow(Window_ *window) {

    short i; 
    for(i = findHighestOpaqueLayer(window); i <= window->index; i++) 
        refreshLayer(&window->layerArray[i], 0);    // do not repaint since we're drawing more stuff on top of this layer
    
    set_graph_callback_to_ram_1();
    repaint_screen_lines(0, VIDEO_Y);
}

void    initializeTextBox(TextBox_ *textbox, short x1, short y1, short x2, short y2, short colour){

    textbox->x1 = x1;
    textbox->x2 = x2;
    textbox->y1 = y1;
    textbox->y2 = y2;
    textbox->colour=colour;

}

void spawnLayer(Layer_ *layer, Window_ *window){

    if(!addLayerToWindow(layer, window)){

        initializeLayer(layer);
    }
}

short  addLayerToWindow(Layer_ *layer, Window_ *window) {

    if(window->index >= MAX_NUM_LAYERS){
        // window full
        //printError("WINDOW FULL");
        return 1;
    }
    else { // add layer to window - aka replacing the pointer to the layer with ours
        window->index++;
        window->layerArray[window->index] = *layer;
        
        return 0;
    }
}

void setLayerTextBox(Layer_ *layer, char *string){

    _strcpy(layer->textBox.body, string);

};

void initializeLayer(Layer_ *layer){

    layer->backgroundColour = COLOR_SH_BLACK;
    layer->index=0;
    layer->visible=1;
}

short getCurrentLayerIndex(Window_ *window){

    return window->index;
}

Window_ *getCurrentWindow(app_data_t *app_data) {

    return(&app_data->mainWindow);
}

short findHighestOpaqueLayer(Window_ *window) {

    short highest = 0;
    for (short i = 0; i <= window->index; i++) {
        if (window->layerArray[i].backgroundColour != COLOR_SH_MASK)
            highest = i;
    }

    return highest;
}

void processTap(Window_ *window, int x, int y) {

    short i;
    button_ temp;
    Layer_ *layer = &window->layerArray[window->index];
    
    for(i = 0; i < layer->index; i++){
        temp = layer->buttonArray[i];
            // was the tap inside the button?
        if (temp.topLeft.x < x && temp.bottomRight.x > x 
                && temp.topLeft.y < y && temp.bottomRight.y > y)
        {   
            vibrate(1,50,0);    // vibrate if successful
            temp.callbackFunction(window, temp);
        }
    }
}

Layer_ *createLayer(void){

    return (Layer_ *)pvPortMalloc(sizeof(Layer_));
}

void destroyLayer(Layer_ *layer) {

    vPortFree(layer);
}

void refreshLayer(Layer_ *layer, short repaint){

    set_bg_color(layer->backgroundColour);
    fill_screen_bg();
    set_graph_callback_to_ram_1();
    
    short i;
    for(i = 0; i < layer->index; i++){

        drawButton(&layer->buttonArray[i]);
    }

    drawTextBox(&layer->textBox);
    
    if(repaint)
        repaint_screen_lines(0, VIDEO_Y);
}

Layer_ *getTopLayer(app_data_t *app_data){

    short top = app_data->mainWindow.index;

    top++;

    return &app_data->mainWindow.layerArray[top];
}

Layer_ *getCurrentLayer(Window_ *window){

    return &window->layerArray[window->index];
}

button_ mirrorInDirectionButton(button_ *button, Way_ dir) {

    button_ temp = *button;
    
    switch(dir){

        case UP:
            
            temp.topLeft.x = abssub(VIDEO_Y, temp.bottomRight.x);
            temp.bottomRight.x = abssub(VIDEO_Y, temp.topLeft.x);
            break;
        
        case DOWN:

            temp.topLeft.x = VIDEO_Y - temp.bottomRight.x;
            temp.bottomRight.x = VIDEO_Y - temp.topLeft.x;
            break;
        
        case LEFT:

            temp.topLeft.y = abssub(VIDEO_X, temp.bottomRight.y);
            temp.bottomRight.y = abssub(VIDEO_X, temp.topLeft.y);
            break;
        
        case RIGHT:

            temp.topLeft.y = VIDEO_X - temp.bottomRight.y;
            temp.bottomRight.y = VIDEO_X - temp.topLeft.y;
            break;

        default:{
            // touch wood
        };

    };

    return temp;
}

button_ moveInDirectionButton(button_ *button, Way_ dir, short separation){
    
    button_ temp    = *button;
    short width     = (short) abssub(temp.topLeft.x, temp.bottomRight.x);     // considering the case when (a,b) is the bottom right point and 
    short height    = (short) abssub(temp.topLeft.y, temp.bottomRight.y);     //      not the top left

    // a = x1, b = y1, c = x2, d = y2

    switch(dir){

        case UP:

            temp.topLeft.y -= (height + separation);
            temp.bottomRight.y -= (height + separation);
            break;
        
        case DOWN:

            temp.topLeft.y += (height + separation);
            temp.bottomRight.y += (height + separation);
            break;
        
        case LEFT:

            temp.topLeft.x -= (width + separation);
            temp.bottomRight.x -= (width + separation);
            break;
        
        case RIGHT:

            temp.topLeft.x += (width + separation);
            temp.bottomRight.x += (width + separation);
            break;

        default:{
            // touch wood
        };

    };

        
    if (temp.topLeft.x < 0)     // display manager will freak out and hang when called with
        temp.topLeft.x = 0;     //       negative values, so this limits the max range
    
    if (temp.topLeft.y < 0)
        temp.topLeft.y = 0;
    
    if (temp.bottomRight.x < 0) 
        temp.bottomRight.x = 0;

    if  (temp.bottomRight.y < 0)
        temp.bottomRight.y = 0;
    
    return temp;
    
}

void spawnButton(button_ *button, Layer_ *layer){

    if(!addButtonToLayer(button, layer)){

        drawButton(button);
    }
}

short addButtonToLayer(button_ *button, Layer_ *layer){

    if(layer->index >= MAX_NUM_BUTTONS){
        // layer full
        //printError("DATABASE FULL");
        return 1;
    }
    else { // add button to layer
        layer->buttonArray[layer->index] = *button;
        layer->index++;
        return 0;
    }
}

void initButton(button_ *button, Point_ topLeft, Point_ bottomRight, char* label, short border,
                    short filling, short text, void* callbackFunction){     // populating the struct

                        button->topLeft.x = topLeft.x;
                        button->topLeft.y = topLeft.y;
                        button->bottomRight.x = bottomRight.x;
                        button->bottomRight.y = bottomRight.y;
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
    draw_filled_rect_bg(    button->topLeft.x,
                            button->topLeft.y,
                            button->bottomRight.x,
                            button->bottomRight.y);
    
    draw_rect(              button->topLeft.x,
                            button->topLeft.y,
                            button->bottomRight.x,
                            button->bottomRight.y);

    //load_font();
    set_fg_color(getLongColour(button->text));

    text_out_center(    button->label,  // the text
                        (int) (button->topLeft.x + button->bottomRight.x) / 2,  // median
                        (int) (button->topLeft.y + button->bottomRight.y) / 2 -10); 

}

void caffeine(Caffeine_t coffee){

    // turn off timeout and exit
  set_display_state_value (8, 1);
  set_display_state_value (2, 1);
  
  if(coffee)    // backlight always on?
    set_display_state_value (4, 1);
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

void setLayerBackground(Layer_ *layer, short colour){

    layer->backgroundColour = colour;

}

short getLayerBackground(Layer_ *layer) {

    return layer->backgroundColour;
}

#endif