/*
*       par_port.c
*
*       This file is part of PlotterController project.
*       https://github.com/berk76/plottercontroller
*
*       PlotterController is free software; you can redistribute it and/or modify
*       it under the terms of the GNU General Public License as published by
*       the Free Software Foundation; either version 3 of the License, or
*       (at your option) any later version. <http://www.gnu.org/licenses/>
*
*       Written by Jaroslav Beran <jaroslav.beran@gmail.com>, on 13.10.2015
*/


/*
 *                                      PLOTTER
 *              PC-printer port        XY4131	XY4140 	      XY4150
 *      bit 2    4 ------------------ 1 ----------- 1 ----------- 1     PEN
 *      bit 3    5 ------------------ 2 ----------- 2 ----------- 2     STEP
 *      bit 1    3 ------------------ 3 ----------- 3 ----------- 3     X/Y
 *      bit 0    2 ------------------ 4 ----------- 4 ----------- 4     +/-
 *      bit 4    6 ------------------ 5 ----------- 5 ----------- 5     READY
 *      GND     18 ------------------ 6 ----------- 6 ----------- 6     GND
 *
 *      The parallel port (or printer port, IEEE1284 standard) has 8 data lines
 *
 *      Pin     Function        Dir
 *      1       /STROBE         W
 *      2..9    DATA Bit0..7    R/W
 *      10      ACK             R
 *      11      /BUSY           R
 *      12      PAPEROUT        R
 *      13      SELECT          R
 *      14      /AUTOFD         W
 *      15      ERROR           R
 *      16      INIT            W
 *      17      /SELECT         W
 *      18..25  GND             -
 *
 *      Signals with a / prefix are low-active (i.e. writing logic 1 to them 
 *      results in 0 Volt output, writing logic 0 results in 5 volts on the output).
 *
 */


#include <stdio.h> 
#include "par_port.h"


#define PEN_BIT 2
#define STEP_BIT 3
#define XY_BIT 1                /* L = CART, H = PAPER */
#define DIRECTION_BIT 0         /* L = TO LEFT (FORWARD), H = TO RIGHT (BACK) */
#define READY_BIT 4             /* L = READY, H = NOT READY */


#define check_bit(var,pos) ((var) & (1<<(pos)))


typedef unsigned char DATA;


/* Static variables */
static DATA data;
static int fd;


/* Static functions */
static void set_bit(DATA *data, int pos, int value);
static int open_parport(char *device);
static int close_parport(int fd);
static int write_data(int fd, unsigned char data);
static int read_data(int fd, unsigned char *data);


int par_open(char *device) {
        data = 0;
        if ((fd = open_parport(device)) == 0) {
                return -1;
        }
        return 0;
}


int par_close() {
        return close_parport(fd);
}


void par_set_pen(int i) {
        set_bit(&data, PEN_BIT, i);
        write_data(fd, data);
}


void par_set_step(int i) {
        set_bit(&data, STEP_BIT, i);
        write_data(fd, data);
}


void par_set_xy(int i) {
        set_bit(&data, XY_BIT, i);
}


int par_is_xy() {
        return check_bit(data, XY_BIT) != 0;
}


void par_set_plus_minus(int i) {
        set_bit(&data, DIRECTION_BIT, i);
}


int par_is_plus_minus() {
        return check_bit(data, DIRECTION_BIT) != 0;
}


void par_set_ready(int i) {
        set_bit(&data, READY_BIT, i);
        write_data(fd, data);
}


int par_is_ready() {
        DATA d;
        read_data(fd, &d);
        return (check_bit(d, READY_BIT) != 0);
}


void set_bit(DATA *data, int pos, int value) {
        if (value == 0) {
                *data &= ~(1<<pos);
        } else {
                *data |= (1<<pos);
        }
}


/*
*       Linux implementation
*       (http://mockmoon-cybernetics.ch/computer/linux/programming/parport.html)
*/

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/ioctl.h> 
#include <linux/parport.h> 
#include <linux/ppdev.h> 


int open_parport(char *device) { 
        int fd; 

        if ((fd=open(device, O_RDWR)) < 0) { 
                perror(device);
                return 0; 
        } 

        if (ioctl(fd, PPCLAIM)) { 
                perror("PPCLAIM"); 
                close(fd); 
                return 0; 
        } 

        return fd;
}


int close_parport(int fd) {
        ioctl(fd, PPRELEASE); 
        close(fd); 
        return 0; 
}


int write_data(int fd, unsigned char data) { 
        return(ioctl(fd, PPWDATA, &data)); 
}


int read_data(int fd, unsigned char *data) { 
        int mode; 

        mode = IEEE1284_MODE_ECP; 
        ioctl(fd, PPSETMODE, &mode);            /* ready to read */ 
        mode = 255; 
        ioctl(fd, PPDATADIR, &mode);            /* read on */ 
        usleep(1000); 
        ioctl(fd, PPRDATA, data);               /* fetch the data */ 
        usleep(1000); 
        mode=0;
        ioctl(fd, PPDATADIR, &mode);            /* write on */
        return 0; 
}

#endif


/*
*       FreeBSD implementation
*       (http://excamera.com/articles/21/parallel.html)
*/

#ifdef __FreeBSD__
#include <fcntl.h>
#include <sys/ioctl.h>
#include <dev/ppbus/ppi.h>
#include <dev/ppbus/ppbconf.h>


int open_parport(char *device) { 
        int fd = open(device, O_RDWR);
        if(fd < 0) {
                perror(device);
                return 0;
        }

        return fd;
}


int close_parport(int fd) {
        close(fd); 
        return 0; 
}


int write_data(int fd, unsigned char data) { 
        if (ioctl(fd, PPISDATA, &data) < 0) {
                perror("ioctl PPISDATA");
                return -1;
        }
        return 0;
}


int read_data(int fd, unsigned char *data) { 
        uint8_t val;
        ioctl(fd, PPIGCTRL, &val);              /* get ctrl register  */
        val |= PCD;
        ioctl(fd, PPISCTRL, &val);              /* read on */
        usleep(1000);
        if (ioctl(fd, PPIGDATA, data) < 0) {    /* fetch the data */
                perror("ioctl PPIGDATA");
                return -1;
        }
        val &= ~PCD;
        ioctl(fd, PPISCTRL, &val);              /* write on */
        usleep(1000);
        return 0;
}

#endif


/*
*       DOS implementation
*/

#ifdef __TURBOC__

#include <dos.h>


int open_parport(char *device) {
        int fd;
        sscanf(device, "%xd", &fd);
        /* Parallel port address */
        return fd;
}


int close_parport(int fd) {
        /* Dummy close */
        return 0;
}


int write_data(int fd, unsigned char data) {
        outport(fd, data);
        return 0;
}


int read_data(int fd, unsigned char *data) {
        write_data(0x37A, 32);          /* read on */
        delay(1000);
        *data = inport(fd);             /* fetch the data */
        write_data(0x37A, 0);           /* write on */
        delay(1000);
        return 0;
}

#endif


/*
*       Access to parallel port doesn't work under MS Windows
*       Needs to be fixed !!!
*/

#ifdef _WIN32
#include <windows.h>
#include <conio.h>


static BOOL bPrivException = FALSE;


LONG WINAPI HandlerExceptionFilter ( EXCEPTION_POINTERS *pExPtrs ) {

        if (pExPtrs->ExceptionRecord->ExceptionCode == EXCEPTION_PRIV_INSTRUCTION) {
                pExPtrs->ContextRecord->Eip ++; // Skip the OUT or IN instruction that caused the exception
                bPrivException = TRUE;
                return EXCEPTION_CONTINUE_EXECUTION;
        } else
                return EXCEPTION_CONTINUE_SEARCH;
}


int open_parport(char *device) {

        SetUnhandledExceptionFilter(HandlerExceptionFilter);
        bPrivException = FALSE;
        _inp(0x378);  // Try to access the given port address

        if (bPrivException) {
        fprintf(stderr, "Privileged instruction exception has occured!\n\n"
                        "To use this program under Windows NT or Windows 2000\n"
                        "you need to install the driver 'UserPort.SYS' and grant\n"
                        "access to the ports used by this program.\n");
                return 0;
        }


        /* To enable Bidirectional data transfer just set the "Bidirectional" bit (bit 5) in control register. */
        write_data(0x37A, 32);

        /* Parallel port address */
        return 0x378;
}


int close_parport(int fd) {
        /* Dummy close */
        return 0;
}


int write_data(int fd, unsigned char data) {
        _outp(fd, data); 
        return 0;
}


int read_data(int fd, unsigned char *data) { 
        *data = _inp(fd); 
        return 0;
}


void usleep_win (long usec) {
        LARGE_INTEGER lFrequency;
        LARGE_INTEGER lEndTime;
        LARGE_INTEGER lCurTime;

        QueryPerformanceFrequency (&lFrequency);
        if (lFrequency.QuadPart) {
                QueryPerformanceCounter (&lEndTime);
                lEndTime.QuadPart += (LONGLONG) usec * lFrequency.QuadPart / 1000000;

                do {
                        QueryPerformanceCounter (&lCurTime);
                        Sleep(0);
                } while (lCurTime.QuadPart < lEndTime.QuadPart);
        }
}

#endif
