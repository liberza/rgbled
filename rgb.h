// rgb.h: header for rgb kernel module
#ifndef RGB_H
#define RGB_H

#define RGBIOCTL_MAGIC		0xB8
#define RGB_SET 			_IOW(RGBIOCTL_MAGIC, 1, colors_t *)
#define RGB_SET_RW			_IOWR(RGBIOCTL_MAGIC, 1, colors_t *)
#define RGB_READ			_IOR(RGBIOCTL_MAGIC, 1, colors_t *)
#define RED					22
#define GREEN   			23
#define BLUE      			24
#define CLK					25

// struct for getting color data from ioctl
typedef struct {
	unsigned int red, green, blue;
} colors_t;

#endif
