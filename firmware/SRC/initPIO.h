#ifndef INITPIO_H_
#define INITPIO_H_


#include "HardwareDefines.hpp"
#include "RGB_Capture.pio.h"


#define InputBufferSize 74880
unsigned char P8D_DISPLAY[InputBufferSize];
char * address_pointer_P8D_DISPLAY = &P8D_DISPLAY[0] ;

static inline uint bits_packed_per_word(uint pin_count) {
    // If the number of pins to be sampled divides the shift register size, we
    // can use the full SR and FIFO width, and push when the input shift count
    // exactly reaches 32. If not, we have to push earlier, so we use the FIFO
    // a little less efficiently.
    const uint SHIFT_REG_WIDTH = 32;
    return SHIFT_REG_WIDTH - (SHIFT_REG_WIDTH % pin_count);
}

void set_PIO_pins(PIO pio) {
    pio_gpio_init(pio, P8D_VSync);
    pio_gpio_init(pio, P8D_HSync);
    pio_gpio_init(pio, P8D_R);
    pio_gpio_init(pio, P8D_G);
    pio_gpio_init(pio, P8D_B);
    pio_gpio_init(pio, TEST_PIN);
}


void RGB_Capture_program_init(PIO pio, uint sm, uint offset) {
    pio_sm_set_consecutive_pindirs(pio, sm, P8D_R, 3, false);
    pio_sm_set_consecutive_pindirs(pio, sm, TEST_PIN, 1, true);
    pio_sm_config c = RGB_Capture_program_get_default_config(offset);
    sm_config_set_in_pins(&c, P8D_R);
    sm_config_set_set_pins(&c, TEST_PIN, 1);
    sm_config_set_in_shift(&c, false, true, 6);
    //sm_config_set_in_shift(&c, true, true, bits_packed_per_word(3));
    //sm_config_set_out_shift(&c, false, true, 3);
    //sm_config_set_jmp_pin(&c,P8D_VSync);
    //sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    sm_config_set_clkdiv(&c, 1.6f);
    pio_sm_init(pio, sm, offset, &c);
}

uint RGB_dma_chan = 0;

void initPIO() {
    uint offset = 0;
    set_PIO_pins(pio1);
    offset = pio_add_program(pio1, &RGB_Capture_program);
    RGB_Capture_program_init(pio1, 0, offset);


    //pio_sm_clear_fifos(pio1, 0);
    //pio_sm_restart(pio1, 0);



    RGB_dma_chan = 4;//dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(RGB_dma_chan);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(pio1, 0, false));
    //channel_config_set_dreq(&c, DREQ_PIO1_RX0);//  pio_get_dreq(pio1, 0, false));
    channel_config_set_transfer_data_size(&c,DMA_SIZE_8);
    channel_config_set_chain_to(&c, RGB_dma_chan+1);                        // chain to other channel

    dma_channel_configure(RGB_dma_chan, &c,
        &P8D_DISPLAY,        // Destination pointer
        &pio1->rxf[0],      // Source pointer
        InputBufferSize,      // Number of transfers
        false                // Start immediately
    );
    //dma_channel_start( RGB_dma_chan);
    pio_sm_put_blocking(pio1, 0, 239);
    pio_sm_set_enabled(pio1, 0, true);
}



void restartINDMA() {
//    gpio_put(TEST_PIN2,!gpio_get(TEST_PIN2));
//    dma_channel_abort( RGB_dma_chan);
    dma_channel_set_write_addr(RGB_dma_chan,&P8D_DISPLAY,true);
//    dma_channel_start( RGB_dma_chan);
}



#endif
