#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "rgb.h"

// Client for RGB LED kernel module
int main(int argc, char *argv[])
{
	int fh;
	colors_t c;
	if (argc != 4) {
		fprintf(stderr, "Usage: %s red green blue\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	// No error checking done on input arguments, that's left to
	// checking errno after calling ioctl for testing purposes
	c.red = atoi(argv[1]);
	c.green = atoi(argv[2]);
	c.blue = atoi(argv[3]);
	fh = open("/dev/rgb", O_WRONLY);
	if (fh < 0) {
		perror("open: ");
		exit(EXIT_FAILURE);
	}
	if (ioctl(fh, RGB_SET, &c) == -1)
		perror("ioctl: ");
	close(fh);
	return 0;
}
