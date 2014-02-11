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
#include <windows.h>
#include <conio.h>
#endif


#ifdef __linux__

int open_parport(char *device) { 
	struct ppdev_frob_struct frob; 
	int fd; int mode; 
	
	if ((fd=open(device, O_RDWR)) < 0) { 
		fprintf(stderr, "can not open %s\n", device); 
		fprintf(stderr, "Probably you don't have permission access %s\n", device);
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
/*
*	Access to parallel port doesn't work under MS Windows
*	Needs to be fixed !!!
*/

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
