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
#ifndef HARDWARE_DEFINES_H_
#define HARDWARE_DEFINES_H_

#include "hardware/vreg.h"


#define P8D_VSync 3
#define P8D_HSync 4
#define P8D_R 0
#define P8D_G 1
#define P8D_B 2

#define TEST_PIN 28
#define TEST_PIN2 29

#define BUTTON_UP 27
#define BUTTON_ENTER 28
#define BUTTON_DOWN 29


#define UART_TX_PIN 12
#define UART_RX_PIN 13
#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE



#define CPU_SPEED 1

#if CPU_SPEED == 1 
    #define CPU_CLOCK_FREQ 200000
    #define CPU_VOLTAGE VREG_VOLTAGE_1_00
#elif CPU_SPEED == 2
    #define CPU_CLOCK_FREQ  250000
    #define CPU_VOLTAGE VREG_VOLTAGE_1_10
#elif CPU_SPEED == 3
    #define CPU_CLOCK_FREQ 300000
    #define CPU_VOLTAGE VREG_VOLTAGE_1_25
#elif CPU_SPEED == 4
    #define CPU_CLOCK_FREQ 350000
    #define CPU_VOLTAGE VREG_VOLTAGE_1_30
#endif





void SetupHardware() {
#if CPU_SPEED > 0    
    // set cpu frequency and voltage
    vreg_set_voltage(CPU_VOLTAGE);
#endif
    stdio_init_all();
#if CPU_SPEED > 0    
    set_sys_clock_khz(CPU_CLOCK_FREQ,true);
#endif




    gpio_init(P8D_VSync);
    gpio_init(P8D_HSync);
    gpio_init(P8D_R);
    gpio_init(P8D_G);
    gpio_init(P8D_B);
    gpio_init(TEST_PIN);
    gpio_init(TEST_PIN2);
    gpio_init(BUTTON_UP);
    gpio_init(BUTTON_ENTER);
    gpio_init(BUTTON_DOWN);

    gpio_set_dir(P8D_VSync,GPIO_IN);
    gpio_set_dir(P8D_HSync,GPIO_IN);
    gpio_set_dir(P8D_R,GPIO_IN);
    gpio_set_dir(P8D_G,GPIO_IN);
    gpio_set_dir(P8D_B,GPIO_IN);
    gpio_set_dir(TEST_PIN,GPIO_OUT);
    gpio_set_dir(TEST_PIN2,GPIO_OUT);
    gpio_set_dir(BUTTON_UP,GPIO_IN);
    gpio_set_dir(BUTTON_ENTER,GPIO_IN);
    gpio_set_dir(BUTTON_DOWN,GPIO_IN);
//    gpio_pull_up(TEST_PIN);
//    gpio_pull_down(P8D_VSync);
//    gpio_pull_down(P8D_HSync);
//    gpio_pull_up(P8D_R);
//    gpio_pull_up(P8D_G);
//    gpio_pull_up(P8D_B);
    gpio_put(TEST_PIN,0);
    gpio_put(TEST_PIN2,0);

    gpio_pull_up(BUTTON_UP);
    gpio_pull_up(BUTTON_ENTER);
    gpio_pull_up(BUTTON_DOWN);


}
#endif