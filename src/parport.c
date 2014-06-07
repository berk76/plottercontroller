/*
*	parport.c
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/


#include <stdio.h> 
#include "parport.h"

/*
*	Linux implementation
*	(http://mockmoon-cybernetics.ch/computer/linux/programming/parport.html)
*/
#ifdef __linux__
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
	ioctl(fd, PPSETMODE, &mode);		/* ready to read */ 
	mode = 255; 
	ioctl(fd, PPDATADIR, &mode); 		/* read on */ 
	usleep(1000); 
	ioctl(fd, PPRDATA, data); 		/* fetch the data */ 
	usleep(1000); 
	mode=0;
	ioctl(fd, PPDATADIR, &mode); 		/* write on */
	return 0; 
}

#endif


/*
*	FreeBSD implementation
*	(http://excamera.com/articles/21/parallel.html)
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
	ioctl(fd, PPIGCTRL, &val);		/* get ctrl register  */
	val |= PCD;
	ioctl(fd, PPISCTRL, &val);		/* read on */
	usleep(1000);
	if (ioctl(fd, PPIGDATA, data) < 0) {	/* fetch the data */
		perror("ioctl PPIGDATA");
		return -1;
	}
	val &= ~PCD;
	ioctl(fd, PPISCTRL, &val);		/* write on */
	usleep(1000);
	return 0;
}

#endif


/*
*	DOS implementation
*	
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
	write_data(0x37A, 32);		/* read on */
	delay(1000);
	*data = inport(fd); 		/* fetch the data */
	write_data(0x37A, 0);		/* write on */
	delay(1000);
	return 0;
}

#endif
