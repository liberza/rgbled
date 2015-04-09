#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "rgb.h"

// Fades from red to green to blue back to red, forking for every write
int main(int argc, char *argv[])
{
	int fh;
	pid_t pid;
	colors_t c;
	c.red = 2047;
	c.green = 0;
	c.blue = 0;

	fh = open("/dev/rgb", O_WRONLY);
	if (fh < 0) {
		perror("open: ");
		exit(EXIT_FAILURE);
	}
	while (c.red > 0) {
		pid = fork();
		if (pid == 0) {
			if (ioctl(fh, RGB_SET, &c) == -1)
				perror("ioctl: ");
			exit(0);
		}
		c.red--;
		c.green++;
	}
	while (c.green > 0) {
		pid = fork();
		if (pid == 0) {
			if (ioctl(fh, RGB_SET, &c) == -1)
				perror("ioctl: ");
			exit(0);
		}
		c.green--;
		c.blue++;
	}
	while (c.blue > 0) {
		pid = fork();
		if (pid == 0) {
			if (ioctl(fh, RGB_SET, &c) == -1)
				perror("ioctl: ");
			exit(0);
		}
		c.blue--;
		c.red++;
	}
	close(fh);
	return 0;
}
