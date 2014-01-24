/*
*	parport.c
*	10.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*	(http://mockmoon-cybernetics.ch/computer/linux/programming/parport.html)
*/


#include <stdio.h> 
#include "parport.h"

#ifdef __linux__
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/ioctl.h> 
#include <linux/parport.h> 
#include <linux/ppdev.h> 
#endif

#ifdef _WIN32
#include <conio.h>
#endif


#ifdef __linux__

int open_parport(char *device) { 
	struct ppdev_frob_struct frob; 
	int fd; int mode; 
	
	if ((fd=open(device, O_RDWR)) < 0) { 
		fprintf(stderr, "can not open %s\n", device); 
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
	int mode, res; 

	mode = IEEE1284_MODE_ECP; 
	res=ioctl(fd, PPSETMODE, &mode);	/* ready to read ? */ 
	mode=255; 
	res=ioctl(fd, PPDATADIR, &mode); 	/* switch output driver off */ 
	usleep(1000); 
	res=ioctl(fd, PPRDATA, data); 		/* now fetch the data! */ 
	usleep(1000); 
	mode=0;
	res=ioctl(fd, PPDATADIR, &mode); 
	return 0; 
}

#endif

#ifdef _WIN32

int open_parport(char *device) { 
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

#endif
