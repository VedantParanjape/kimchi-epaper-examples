#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "spi.h"

int main(void) {
    spi_t *spi;
    uint8_t buf[4] = { 0xaa, 0xbb, 0xcc, 0xdd };

    spi = spi_new();

    /* Open spidev1.0 with mode 0 and max speed 1MHz */
    if (spi_open(spi, "/dev/spidev1.0", 0, 1000000) < 0) {
        fprintf(stderr, "spi_open(): %s\n", spi_errmsg(spi));
        exit(1);
    }

    /* Shift out and in 4 bytes */
    if (spi_transfer(spi, buf, buf, sizeof(buf)) < 0) {
        fprintf(stderr, "spi_transfer(): %s\n", spi_errmsg(spi));
        exit(1);
    }

    printf("shifted in: 0x%02x 0x%02x 0x%02x 0x%02x\n", buf[0], buf[1], buf[2], buf[3]);

    spi_close(spi);

    spi_free(spi);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gpio.h"

int main(void) {
    gpio_t *gpio_in, *gpio_out;
    bool value;

    gpio_in = gpio_new();
    gpio_out = gpio_new();

    /* Open GPIO /dev/gpiochip0 line 10 with input direction */
    if (gpio_open(gpio_in, "/dev/gpiochip0", 10, GPIO_DIR_IN) < 0) {
        fprintf(stderr, "gpio_open(): %s\n", gpio_errmsg(gpio_in));
        exit(1);
    }

    /* Open GPIO /dev/gpiochip0 line 12 with output direction */
    if (gpio_open(gpio_out, "/dev/gpiochip0", 12, GPIO_DIR_OUT) < 0) {
        fprintf(stderr, "gpio_open(): %s\n", gpio_errmsg(gpio_out));
        exit(1);
    }

    /* Read input GPIO into value */
    if (gpio_read(gpio_in, &value) < 0) {
        fprintf(stderr, "gpio_read(): %s\n", gpio_errmsg(gpio_in));
        exit(1);
    }

    /* Write output GPIO with !value */
    if (gpio_write(gpio_out, !value) < 0) {
        fprintf(stderr, "gpio_write(): %s\n", gpio_errmsg(gpio_out));
        exit(1);
    }

    gpio_close(gpio_in);
    gpio_close(gpio_out);

    gpio_free(gpio_in);
    gpio_free(gpio_out);

    return 0;
}