/*********************************************************************
 * gpio_io.h : GPIO Access Code
 *********************************************************************/

typedef enum {
    Input = 0,                  /* GPIO is an Input */
    Output                      /* GPIO is an Output */
} direction_t;


/*********************************************************************
 * Perform initialization to access GPIO registers:
 * Sets up pointer ugpio.
 *********************************************************************/
extern void gpio_init(int version);
    
/*********************************************************************
 * Configure GPIO as Input or Output
 *********************************************************************/
extern inline void gpio_config(int gpio,direction_t output);

/*********************************************************************
 * Write a bit to the GPIO pin
 *********************************************************************/
extern inline void gpio_write(int gpio,int bit);
    
/*********************************************************************
 * Read a bit from a GPIO pin
 *********************************************************************/
extern inline int gpio_read(int gpio);
    
/*********************************************************************
 * End gpio_io.h - by Warren Gay
 * Mastering the Raspberry Pi - ISBN13: 978-1-484201-82-4
 * This source code is placed into the public domain.
 *********************************************************************/
 
