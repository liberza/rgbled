#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "rgb.h"

// Tests rgb driver for correct output
// See testcases.txt for individual test details

int main(int argc, char *argv[])
{
	int fh;
	colors_t c;

	fh = open("/dev/rgb", O_WRONLY);
	if (fh < 0) {
		perror("open: ");
		exit(EXIT_FAILURE);
	}
	
	// Tests:
	// 1
	errno = 0;
	if (ioctl(fh, RGB_SET, &c) < 0) {
		if (errno == EINVAL) printf("1: pass\n");
		else printf("1: fail\n");
	}
	else printf("1: fail\n");

	// 2
	errno = 0;
	c.red = -1;
	c.green = -1;
	c.blue = -1;
	if (ioctl(fh, RGB_SET, &c) < 0) {
		if (errno == EINVAL) printf("2: pass\n");
		else printf("2: fail\n");
	}
	else printf("2: fail\n");
	// 3
	errno = 0;
	c.red = -1;
	c.green = 0;
	c.blue = 0;
	if (ioctl(fh, RGB_SET, &c) < 0) {
		if (errno == EINVAL) printf("3: pass\n");
		else printf("3: fail\n");
	}
	else printf("3: fail\n");

	// 4
	errno = 0;
	c.red = 0;
	c.green = -1;
	c.blue = 0;
	if (ioctl(fh, RGB_SET, &c) < 0) {
		if (errno == EINVAL) printf("4: pass\n");
		else printf("4: fail\n");
	}
	else printf("4: fail\n");

	// 5
	errno = 0;
	c.red = 0;
	c.green = 0;
	c.blue = -1;
	if (ioctl(fh, RGB_SET, &c) < 0) {
		if (errno == EINVAL) printf("5: pass\n");
		else printf("5: fail\n");
	}
	else printf("5: fail\n");

	// 6
	errno = 0;
	c.red = 2048;
	c.green = 2048;
	c.blue = 2048;
	if (ioctl(fh, RGB_SET, &c) < 0) {
		if (errno == EINVAL) printf("6: pass\n");
		else printf("6: fail\n");
	}
	else printf("6: fail\n");

	// 7
	errno = 0;
	c.red = 2048;
	c.green = 2047;
	c.blue = 2047;
	if (ioctl(fh, RGB_SET, &c) < 0) {
		if (errno == EINVAL) printf("7: pass\n");
		else printf("7: fail\n");
	}
	else printf("7: fail\n");

	// 8
	errno = 0;
	c.red = 2047;
	c.green = 2048;
	c.blue = 2047;
	if (ioctl(fh, RGB_SET, &c) < 0) {
		if (errno == EINVAL) printf("8: pass\n");
		else printf("8: fail\n");
	}
	else printf("8: fail\n");

	// 9
	errno = 0;
	c.red = 2047;
	c.green = 2047;
	c.blue = 2048;
	if (ioctl(fh, RGB_SET, &c) < 0) {
		if (errno == EINVAL) printf("9: pass\n");
		else printf("9: fail\n");
	}
	else printf("9: fail\n");

	// 10
	errno = 0;
	c.red = 2047;
	c.green = 2047;
	c.blue = 2047;
	if (ioctl(fh, RGB_SET_RW, &c) < 0) {
		if (errno == ENOTTY) printf("10: pass\n");
		else printf("10: fail\n");
	}
	else printf("10: fail\n");

	// 11
	errno = 0;
	if (ioctl(fh, RGB_READ, &c) < 0) {
		if (errno == ENOTTY) printf("11: pass\n");
		else printf("11: fail\n");
	}
	else printf("11: fail\n");
	
	// 12
	errno = 0;
	if ((write(fh, NULL, 1)) < 0) {
		if (errno == ENOTSUP) printf("12: pass\n");
		else printf("12: fail\n");
	}
	else printf("12: fail\n");
	
	
	// 13
	errno = 0;
	close(fh);
	// open as read-only
	fh = open("/dev/rgb", O_RDONLY);
	// handle user not having read access to device file, as they shouldn't.
	if ((errno == ENOTSUP) || (errno == EACCES)) printf("13: pass\n");
	else printf("13: fail\n");
	close(fh);
	// open as write-only for next tests
	fh = open("/dev/rgb", O_WRONLY);
	if (fh < 0) {
		perror("open: ");
		exit(EXIT_FAILURE);
	}
	
	// 14 
	c.red = 2047;
	c.green = 2047;
	c.blue = 2047;
	if (ioctl(fh, RGB_SET, &c) < 0) printf("14: fail\n");
	else printf("14: pass (LED white)\n");
	printf("Press enter to continue");
	getchar();
	
	
	// 15
	c.red = 2047;
	c.green = 0;
	c.blue = 0;
	if (ioctl(fh, RGB_SET, &c) < 0) printf("15: fail\n");
	else printf("15: pass (LED red)\n");
	printf("Press enter to continue");
	getchar();
	

	// 16
	c.red = 0;
	c.green = 2047;
	c.blue = 0;
	if (ioctl(fh, RGB_SET, &c) < 0) printf("16: fail\n");
	else printf("16: pass (LED green)\n");
	printf("Press enter to continue");
	getchar();
	

	// 17
	c.red = 0;
	c.green = 0;
	c.blue = 2047;
	if (ioctl(fh, RGB_SET, &c) < 0) printf("17: fail\n");
	else printf("17: pass (LED blue)\n");
	printf("Press enter to continue");
	getchar();
	
	// 18
	c.red = 0;
	c.green = 0;
	c.blue = 0;
	if (ioctl(fh, RGB_SET, &c) < 0) printf("18: fail\n");
	else printf("18: pass (LED off)\n");
	printf("Press enter to continue");
	getchar();

	// 19
	errno = 0;
	if (ioctl(fh, RGB_SET, NULL) < 0) {
		if (errno == EFAULT) printf("19: pass\n");
        else printf("19: fail\n");
    }
    else printf("19: fail\n");
	
	// 20 
	errno = 0;
	close(fh);
	// open as read/write
	fh = open("/dev/rgb", O_RDWR);
	// handle user not having read access to device file
	if ((errno == ENOTSUP) || (errno == EACCES)) printf("20: pass\n");
	else printf("20: fail\n");
	
	close(fh);
	return 0;
}
