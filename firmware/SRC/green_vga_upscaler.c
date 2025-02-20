/**
 * Hunter Adams (vha3@cornell.edu)
 * 
 *
 * HARDWARE CONNECTIONS
 *  - GPIO 16 ---> VGA Hsync
 *  - GPIO 17 ---> VGA Vsync
 *  - GPIO 18 ---> 330 ohm resistor ---> VGA Red
 *  - GPIO 19 ---> 330 ohm resistor ---> VGA Green
 *  - GPIO 20 ---> 330 ohm resistor ---> VGA Blue
 *  - RP2040 GND ---> VGA GND
 *
 * RESOURCES USED
 *  - PIO state machines 0, 1, and 2 on PIO instance 0
 *  - DMA channels 0, 1, 2, and 3
 *  - 153.6 kBytes of RAM (for pixel color data)
 *
 */

// VGA graphics library
#include "vga_graphics.h"
#include <stdio.h>
#include <stdlib.h>
// #include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "pico/multicore.h"

#include "HardwareDefines.hpp"
#include "initPIO.h"
#include "images/logo.h"
#include "string.h"

uint16_t X_SCAN_POS = 0;
uint16_t Y_SCAN_POS = 0;

#define Y_CORRECTION 42 //48
uint8_t X_CORRECTION = 26;


void SetupUART();


bool osd = false;
uint32_t osd_timer = 0;
uint8_t current_pos = 0;

#define OSD_LEFT 200
#define OSD_TOP 100
#define OSD_WIDTH 200
#define OSD_HEIGHT 220

uint8_t fps = 0;
char ROM_0[16];
char ROM_1[16];
char ROM_2[16];
char ROM_3[16];
char ROM_4[16];
char ROM_5[16];
char ROM_6[16];
char ROM_7[16];
void core1_procedure() {
    uint8_t y = 0;
    while(1) {
        for (uint16_t yp=Y_CORRECTION;yp<(235+Y_CORRECTION);yp++) {
            y = yp - Y_CORRECTION;
            if ((osd)&&(y*2 >= OSD_TOP)&&(y*2 <= OSD_TOP+OSD_HEIGHT))
                for (uint8_t x=0;x<240;x++) {
                    //copyLineToFrameBuffer(y,address_pointer_P8D_DISPLAY,240);

    /*
                    if (
                        (osd)
                        &&(X_CORRECTION+x*2 >= OSD_LEFT ) 
                        &&(X_CORRECTION+x*2 <= OSD_LEFT+OSD_WIDTH )  
                        &&(y*2 >= OSD_TOP)
                        &&(y*2 <= OSD_TOP+OSD_HEIGHT)
                        ){

                    } else 
    */                
                    drawPixel2(X_CORRECTION+x*2  ,y*2,  P8D_DISPLAY[yp*240+x]);
                }
        }
        if (osd) {
                fillRect(OSD_LEFT, OSD_TOP, OSD_WIDTH, OSD_HEIGHT, BLACK); // blue box
                drawRect(OSD_LEFT,OSD_TOP,OSD_WIDTH,OSD_HEIGHT,GREEN);
                drawRect(OSD_LEFT+1,OSD_TOP+1,OSD_WIDTH-2,OSD_HEIGHT-2,GREEN);
                //char s[32];
                //sprintf(s,"PRAVETZ-8D");
                setTextColor(WHITE) ;
                setTextSize(2) ;
                setCursor(OSD_LEFT+20, OSD_TOP+10) ;
                writeString(ROM_0) ;
                setCursor(OSD_LEFT+20, OSD_TOP+30) ;
                writeString(ROM_1) ;
                setCursor(OSD_LEFT+20, OSD_TOP+50) ;
                writeString(ROM_2) ;
                setCursor(OSD_LEFT+20, OSD_TOP+70) ;
                writeString(ROM_3) ;
                setCursor(OSD_LEFT+20, OSD_TOP+90) ;
                writeString(ROM_4) ;
                setCursor(OSD_LEFT+20, OSD_TOP+110) ;
                writeString(ROM_5) ;
                setCursor(OSD_LEFT+20, OSD_TOP+130) ;
                writeString(ROM_6) ;
                setCursor(OSD_LEFT+20, OSD_TOP+150) ;
                writeString(ROM_7) ;
                setCursor(OSD_LEFT+20, OSD_TOP+170) ;
                writeString("WIDE SCREEN") ;
                setCursor(OSD_LEFT+20, OSD_TOP+190) ;
                writeString("3:4") ;
                setTextColor(BLUE) ;
                //char s[32];
                setCursor(OSD_LEFT+2, OSD_TOP+10+current_pos*20) ;
                //sprintf(s,">");
                writeString(">") ;
                setCursor(OSD_LEFT+OSD_WIDTH-20, OSD_TOP+10+current_pos*20) ;
                //sprintf(s,"<");
                writeString("<") ;

        }
        fps++;
    }
}

char * FB_address_pointer = 0;

void Sync_irq_cb(uint gpio, uint32_t events) {
//    dma_channel_set_write_addr(RGB_dma_chan,FB_address_pointer,true);
    dma_channel_set_write_addr(RGB_dma_chan,&P8D_DISPLAY,true);
}

#define FPS_X_POS 430
#define FPS_Y_POS 470   
#define FPS_FONT_SIZE 1

bool repeating_timer_callback(__unused struct repeating_timer *t) {
                fps=0;
                fillRect(FPS_X_POS, FPS_Y_POS, FPS_X_POS+100, FPS_Y_POS+50, BLACK); // blue box
                setCursor(FPS_X_POS, FPS_Y_POS) ;
                char s[30];
                sprintf(s,"F:%i",fps);
                setTextColor(GREEN) ;
                setTextSize(FPS_FONT_SIZE) ;
                writeString(s) ;
    return true;
}

int main() {

    // Initialize stdio
    SetupHardware();
    SetupUART();
    initPIO();
    initVGA() ;

    sprintf(ROM_0,"PRAVETZ-8D");
    sprintf(ROM_1,"ORIC");
    sprintf(ROM_2,"-");
    sprintf(ROM_3,"-");
    sprintf(ROM_4,"-");
    sprintf(ROM_5,"-");
    sprintf(ROM_6,"-");
    sprintf(ROM_7,"DIAGNOSTIC");

    for(int y=0;y<240;y++) {
        for(int x=0;x<320;x++) {
            drawPixel(x+150,y+130,LOGO[y*320+x]);
        }
    }

    /*
    setTextColor(GREEN) ;
    setCursor(65, 100) ;
    setTextSize(6) ;
    writeString("GreenHamr") ;
    setCursor(65, 200) ;
    writeString("PRAVETZ - 8D") ;
*/
    sleep_ms(5000);

    FB_address_pointer = getFrameBufferAddressPointer();
/*
    // circle radii
    short circle_x = 0 ;

    // color chooser
    char color_index = 0 ;

    // position of the disc primitive
    short disc_x = 0 ;
    // position of the box primitive
    short box_x = 0 ;
    // position of vertical line primitive
    short Vline_x = 350;
    // position of horizontal line primitive
    short Hline_y = 250;

    // Draw some filled rectangles
    fillRect(64, 0, 176, 50, BLUE); // blue box
    fillRect(250, 0, 176, 50, RED); // red box
    fillRect(435, 0, 176, 50, GREEN); // green box

    // Write some text
    setTextColor(WHITE) ;
    setCursor(65, 0) ;
    setTextSize(1) ;
    writeString("Raspberry Pi Pico") ;
    setCursor(65, 10) ;
    writeString("Graphics primitives demo") ;
    setCursor(65, 20) ;
    writeString("Hunter Adams") ;
    setCursor(65, 30) ;
    writeString("vha3@cornell.edu") ;
    setCursor(250, 0) ;
    setTextSize(2) ;
    writeString("Time Elapsed:") ;
*/
/*
    // Setup a 1Hz timer
    struct repeating_timer timer;
    add_repeating_timer_ms(-1000, repeating_timer_callback, NULL, &timer);
*/

    gpio_set_irq_enabled_with_callback(P8D_VSync, GPIO_IRQ_EDGE_FALL , true, &Sync_irq_cb);
    //gpio_set_irq_enabled(P8D_HSync, GPIO_IRQ_EDGE_RISE, true);

    multicore_launch_core1(core1_procedure);

    pio_sm_set_clkdiv(pio1, 0, 1.587f);


    uint8_t button_up_state = false;
    uint8_t button_down_state = false;
    uint8_t button_enter_state = false;
    float Freq = 7.0f;
    pio_sm_set_clkdiv(pio0, 0, 7.0f);


    struct repeating_timer timer;
    //add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &timer);
#define X_POS 430
#define Y_POS 470   
#define FONT_SIZE 1
    while(true) {
/*
        for (uint16_t yp=Y_CORRECTION;yp<(235+Y_CORRECTION);yp++) {
            for (uint8_t x=0;x<120;x++) {
                uint8_t y = yp - Y_CORRECTION;
                drawPixel(X_CORRECTION+x*2  ,y*2,  P8D_DISPLAY[yp*240+x]);
                drawPixel(X_CORRECTION+x*2+1,y*2,  P8D_DISPLAY[yp*240+x]);
                drawPixel(X_CORRECTION+x*2  ,y*2+1,P8D_DISPLAY[yp*240+x]);
                drawPixel(X_CORRECTION+x*2+1,y*2+1,P8D_DISPLAY[yp*240+x]);
            }
        }
*/
        if (osd) osd_timer++;
        if (osd_timer > 50000000) osd = false;

         if (gpio_get(BUTTON_UP)==0) {
            if (button_up_state == false) {
                button_up_state = true;
                osd = true;
                osd_timer = 0;
                if (current_pos>0) current_pos--;
                /*
                Freq += 0.01;         
                fillRect(X_POS, Y_POS, X_POS+100, Y_POS+50, BLACK); // blue box
                setCursor(X_POS, Y_POS) ;
                char s[30];
                sprintf(s,"F:68Hz");
                setTextColor(GREEN) ;
                setTextSize(FONT_SIZE) ;
                writeString(s) ;
                pio_sm_set_clkdiv(pio0, 0, 7.0f);
                */
            }    
         } else {
            button_up_state = false;
         }

         if (gpio_get(BUTTON_DOWN)==0) {
            if (button_down_state == false) {
                button_down_state = true;
                osd = true;
                osd_timer = 0;
                if (current_pos<9) current_pos++;

                /*
                //Freq -= 0.01;         
                fillRect(X_POS, Y_POS, X_POS+100, Y_POS+50, BLACK); // blue box
                setCursor(X_POS, Y_POS) ;
                char s[30];
                sprintf(s,"F:60Hz");
                setTextColor(GREEN) ;
                setTextSize(FONT_SIZE) ;
                writeString(s) ;
                pio_sm_set_clkdiv(pio0, 0, 8.0f);
                */
            }    
         } else {
            button_down_state = false;
         }

         if (gpio_get(BUTTON_ENTER)==0) {
            if (button_enter_state == false) {
                button_enter_state = true;
                if (current_pos<8) {
                    char s[30];
                    sprintf(s,"S%i\r\n",current_pos);
                    uart_puts(UART_ID,s);
                    uart_get_hw(UART_ID)->icr = UART_UARTICR_TXIC_BITS ;
                }
                if (current_pos == 8) {
                    X_CORRECTION = 26;
                    pio_sm_set_clkdiv(pio0, 0, 7.0f);
                    sleep_ms(1000);
                    fillRect(0, 0, 639, 479, BLACK); // blue box
                }
                if (current_pos == 9) { 
                    X_CORRECTION = 66;
                    pio_sm_set_clkdiv(pio0, 0, 8.0f);
                    sleep_ms(1000);
                    fillRect(0, 0, 639, 479, BLACK); // blue box
                }
                osd = false;
                osd_timer = 0;
            }    
         } else {
            button_enter_state = false;
         }

   }

}


char rcv_buffer[64];
uint8_t rcv_pos = 0;

void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);
        // Can we send it back?
        
        if (ch==13) {
            rcv_buffer[rcv_pos++] = 0;
            if (rcv_buffer[0] == 76) {
                if (rcv_buffer[1] == 48) sprintf(ROM_0,"%s",rcv_buffer+2);
                if (rcv_buffer[1] == 49) sprintf(ROM_1,"%s",rcv_buffer+2);
                if (rcv_buffer[1] == 50) sprintf(ROM_2,"%s",rcv_buffer+2);
                if (rcv_buffer[1] == 51) sprintf(ROM_3,"%s",rcv_buffer+2);
                if (rcv_buffer[1] == 52) sprintf(ROM_4,"%s",rcv_buffer+2);
                if (rcv_buffer[1] == 53) sprintf(ROM_5,"%s",rcv_buffer+2);
                if (rcv_buffer[1] == 54) sprintf(ROM_6,"%s",rcv_buffer+2);
                if (rcv_buffer[1] == 55) sprintf(ROM_7,"%s",rcv_buffer+2);
            }
            rcv_pos = 0;
        } else {
            //setCursor(X_POS-300+5*rcv_pos, Y_POS) ;
            //setTextColor(GREEN) ;
            //setTextSize(FONT_SIZE) ;
            //writeString(".") ;
            rcv_buffer[rcv_pos++] = ch;
        }
        
    }
}

void SetupUART() {
    uart_init(UART_ID, 115200);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);
    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);
    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, true);
    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);
    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);   
}