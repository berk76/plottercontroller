/*
*	gpio_port.c
*	12.10.2015
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/


#include <stdio.h>
#include <unistd.h>
#include "gpio_io.h"


#define GPIO_PEN                10
#define GPIO_STEP               11
#define GPIO_XY                 14
#define GPIO_PLUS_MINUS         15
#define GPIO_SET_READY          17
#define GPIO_GET_READY          18


static int status_xy;
static int status_plus_minus;


int gpio_open(int version) {
        gpio_init(version);
        
        gpio_config(GPIO_PEN, Output);
        gpio_write(GPIO_PEN, 0);
        
        gpio_config(GPIO_STEP, Output);
        gpio_write(GPIO_STEP, 0);
        
        gpio_config(GPIO_XY, Output);
        gpio_write(GPIO_XY, 0);
        status_xy = 0;
        
        gpio_config(GPIO_PLUS_MINUS, Output);
        gpio_write(GPIO_XY, 0);
        status_plus_minus = 0;
        
        gpio_config(GPIO_SET_READY, Output);
        gpio_write(GPIO_SET_READY, 0);
        
        gpio_config(GPIO_GET_READY, Input);
        
        return 0;
}


int gpio_close() {
        return 0;
}


void gpio_set_pen(int i) {
        gpio_write(GPIO_PEN, i);
}


void gpio_set_step(int i) {
        gpio_write(GPIO_STEP, i);
}


void gpio_set_xy(int i) {
        gpio_write(GPIO_XY, i);
        status_xy = i;
}


int gpio_is_xy() {
        return status_xy;
}


void gpio_set_plus_minus(int i) {
        gpio_write(GPIO_PLUS_MINUS, i);
        status_plus_minus = i;
}


int gpio_is_plus_minus() {
        return status_plus_minus;
}


void gpio_set_ready(int i) {
        gpio_write(GPIO_SET_READY, i);
}


int gpio_is_ready() {
	return gpio_read(GPIO_GET_READY);
}

