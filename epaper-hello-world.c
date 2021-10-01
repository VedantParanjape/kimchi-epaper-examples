#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "spi.h"
#include "gpio.h"
#include "image-data.h"

spi_t *spi;
gpio_t *rst_pin, *dc_pin, *busy_pin;

void millisleep(uint time) {usleep(time*1000);}

void send_command(uint8_t command)
{
    gpio_write(dc_pin, false);

    uint8_t buf[1] = { command };
    /* Shift out and in 4 bytes */
    if (spi_transfer(spi, buf, buf, 1) < 0) {
        fprintf(stderr, "spi_transfer(): %s\n", spi_errmsg(spi));
    }
    // gpio_write(dc_pin, true);
}

void send_data(uint8_t command)
{ 
    gpio_write(dc_pin, true);

    uint8_t buf[1] = { command };
    /* Shift out and in 4 bytes */
    if (spi_transfer(spi, buf, buf, 1) < 0) {
        fprintf(stderr, "spi_transfer(): %s\n", spi_errmsg(spi));
    }
    // gpio_write(cs_pin, true);
}

void wait_until_idle()
{
    bool busy;
    printf("e-Paper busy\n");
    uint count = 0;
    do
    {
        send_command(0x71);
        gpio_read(busy_pin, &busy);
        count++;
    }
    while(!busy && count < 100);
    printf("e-Paper busy release\n");
    millisleep(20);
}

void epd_sleep()
{
    send_command(0x50);
    send_data(0xf7);
    send_command(0x02);

    wait_until_idle();

    send_command(0x07);
    send_data(0xA5);
}

void epd_reset()
{
    uint module_reset_timing = 50; 

    gpio_write(rst_pin, false); 
    millisleep(module_reset_timing);
    gpio_write(rst_pin, true); 
    millisleep(module_reset_timing);

    gpio_write(rst_pin, false); 
    millisleep(module_reset_timing);
    gpio_write(rst_pin, true); 
    millisleep(module_reset_timing);
    
    gpio_write(rst_pin, false); 
    millisleep(module_reset_timing);
    gpio_write(rst_pin, true); 
    millisleep(module_reset_timing);
}

void epd_refresh()
{
    send_command(0x12);
    millisleep(1000);
    wait_until_idle();
}

int main(void) {
    int width = 128 / 8;
    int height = 296;
    
    spi = spi_new();
    rst_pin = gpio_new();
    dc_pin = gpio_new();
    busy_pin = gpio_new();

    // setup gpio
    // RST_PIN output
    // MX8MM_IOMUXC_SAI3_TXFS_GPIO4_IO31
    if (gpio_open(rst_pin, "/dev/gpiochip3", 31, GPIO_DIR_OUT) < 0) {
        fprintf(stderr, "gpio_open(): %s\n", gpio_errmsg(rst_pin));
        exit(1);
    }

    // DC_PIN output
    // MX8MM_IOMUXC_SAI3_RXC_GPIO4_IO29
    if (gpio_open(dc_pin, "/dev/gpiochip3", 29, GPIO_DIR_OUT) < 0) {
        fprintf(stderr, "gpio_open(): %s\n", gpio_errmsg(dc_pin));
        exit(1);
    }

    // BUSY_PIN input
    // MX8MM_IOMUXC_SAI3_MCLK_GPIO5_IO2
    if (gpio_open(busy_pin, "/dev/gpiochip4", 2, GPIO_DIR_IN) < 0) {
        fprintf(stderr, "gpio_open(): %s\n", gpio_errmsg(busy_pin));
        exit(1);
    }

    /* Open spidev1.0 with mode 0 and max speed 1MHz */
    if (spi_open(spi, "/dev/spidev0.0", 0, 2000000) < 0) {
        fprintf(stderr, "spi_open(): %s\n", spi_errmsg(spi));
        exit(1);
    }

// while(1)
// {
//     gpio_write(rst_pin, true);
//     gpio_write(dc_pin, true);
//     gpio_write(busy_pin, true);

//     millisleep(4000);

//     gpio_write(rst_pin, false);
//     gpio_write(dc_pin, false);
//     gpio_write(busy_pin, false);

//     millisleep(4000);
// }

    printf("started program\n");

    epd_reset();
    printf("reset\n");

    //power on
    send_command(0x04);
    printf("power on\n");

    // WAITUNTILIDLE()
    wait_until_idle();

    send_command(0x00); // panel setting
    printf("panel setting\n");
    send_data(0x0f); // lut from otp, 128x296
    printf("default data\n");
    send_data(0x89); // Temperature sensor, boost and other related timing settings
    printf("timing settings\n");

    send_command(0x61); //Display resolution setting
    printf("display resolution\n");
    send_data(0x80);
    send_data(0x01);
    send_data(0x28);
    printf("display resolution over\n");

    send_command(0x50);//VCOM AND DATA INTERVAL SETTING      
    printf("vcom and data interval\n");
    send_data(0x77);//WBmode:VBDF 17|D7 VBDW 97 VBDB 57   
    printf("WB mode\n");
                   //WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

    // EPD Clear
    //send black data
    send_command(0x10);
    for (uint j = 0; j < height; j++) {
        for (uint i = 0; i < width; i++) {
            send_data(0x00);
        }
    }

    //send red data
    send_command(0x13);
    
    for (uint i = 0; i < height*width; i++) {
        send_data(gImage_R1[i]);
    }

    // for (uint j = 0; j < height; j++) {
    //     for (uint i = 0; i < width; i++) {
    //         send_data(0x00);
    //     }
    // }

    epd_refresh();
    epd_sleep();
    
    spi_close(spi);
    spi_free(spi);

    gpio_close(rst_pin);
    gpio_close(dc_pin);
    gpio_close(busy_pin);
    gpio_free(rst_pin);
    gpio_free(dc_pin);
    gpio_free(busy_pin);

    return 0;
}