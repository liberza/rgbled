#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#define RGBIOCTL_MAGIC 0xB8
#define RGB_SET _IOW(RGBIOCTL_MAGIC, 1, colors_t *)

int main(int argc, char *argv[])
{
	typedef struct {
	int red, green, blue;
	} colors_t;

	int fh;
	colors_t c;
	if (argc != 4) {
		fprintf(stderr, "Usage: %s red green blue\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	c.red = atoi(argv[1]);
	c.green = atoi(argv[2]);
	c.blue = atoi(argv[3]);
	printf("%d %d %d\n", c.red, c.green, c.blue);
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
