#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

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
	fh = open("/dev/rgb", O_WRONLY);
	if (fh < 0) {
		perror("open: ");
		exit(EXIT_FAILURE);
	}
	ioctl(fh, (_IOWR('c', 3, colors_t *)), &c);
	close(fh);
	return 0;
}
