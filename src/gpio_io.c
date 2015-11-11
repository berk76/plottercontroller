/*********************************************************************
 * gpio_io.c : GPIO Access Code
 * https://github.com/ve3wwg/raspberry_pi/blob/master/dht11/gpio_io.c 
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "gpio_io.h"


/* #define BCM2708_PERI_BASE       0x20000000 */
/* #define BCM2708_PERI_BASE       0x3f000000 */
static off_t BCM2708_PERI_BASE;

#define GPIO_BASE               (BCM2708_PERI_BASE + 0x200000)
#define BLOCK_SIZE (4*1024)

/* GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x)
   or SET_GPIO_ALT(x,y) */
#define INP_GPIO(g) *(ugpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(ugpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) \
    *(ugpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(ugpio+7)     /* sets   bits */
#define GPIO_CLR *(ugpio+10)    /* clears bits */
#define GPIO_GET *(ugpio+13)    /* gets   all GPIO input levels */

static volatile unsigned *ugpio;


/*********************************************************************
 * Perform initialization to access GPIO registers:
 * Sets up pointer ugpio.
 *********************************************************************/
void
gpio_init(int version) {
    int fd;	
    char *map;
    
    switch (version) {
        case 1:         BCM2708_PERI_BASE = 0x20000000;
                        break;
        case 2:         BCM2708_PERI_BASE = 0x3f000000;
                        break;
        default:        fprintf(stderr, "GPIO version %d is invalid\n", version); 
                        exit(1);
    }

    fd = open("/dev/mem",O_RDWR|O_SYNC);  /* Needs root access */
    if ( fd < 0 ) {
        perror("Opening /dev/mem");
        exit(1);
    }

    map = (char *) mmap(
        NULL,             /* Any address */
        BLOCK_SIZE,       /* # of bytes */
        PROT_READ|PROT_WRITE,
        MAP_SHARED,       /* Shared */
        fd,               /* /dev/mem */
        GPIO_BASE         /* Offset to GPIO */
    );

    if ( (long)map == -1L ) {
        perror("mmap(/dev/mem)");    
        exit(1);
    }

    close(fd);
    ugpio = (volatile unsigned *)map;
}

/*********************************************************************
 * Configure GPIO as Input or Output
 *********************************************************************/
inline void
gpio_config(int gpio,direction_t output) {
    INP_GPIO(gpio);
    if ( output ) {
        OUT_GPIO(gpio);
    }
}

/*********************************************************************
 * Write a bit to the GPIO pin
 *********************************************************************/
inline void
gpio_write(int gpio,int bit) {
    unsigned sel = 1 << gpio;

    if ( bit ) {
        GPIO_SET = sel;
    } else  {
        GPIO_CLR = sel;
    }
}

/*********************************************************************
 * Read a bit from a GPIO pin
 *********************************************************************/
inline int
gpio_read(int gpio) {
    unsigned sel = 1 << gpio;

    return (GPIO_GET) & sel ? 1 : 0;
}

/*********************************************************************
 * End gpio_io.c - by Warren Gay
 * Mastering the Raspberry Pi - ISBN13: 978-1-484201-82-4
 * This source code is placed into the public domain.
 *********************************************************************/
 
