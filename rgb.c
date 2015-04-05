#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>

#define DRIVER_AUTHOR	"Nick Levesque <nick.levesque@gmail.com>"
#define DRIVER_DESC	"Sets red, green and blue values for external LED"
#define DEVICE_NAME	"rgb"
struct rgb_dev {
	int ret;
	dev_t dev_num;
	struct cdev *cdev;
	int major_num;
} rgbdev = {
	.major_num = 0,
	.ret = 0,
	.dev_num = 0,
};

// Implementation of file operation methods

int rgb_open(struct inode *inode, struct file *filp)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgb: opened device\n");
	#endif
	return 0;
}

// Allow threads to read
int rgb_read(struct file *filp, char *buf, size_t buf_cnt, loff_t* offset)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgb read from device\n");
	#endif
	return 0;
}

// Allow threads to write colors to LED
ssize_t rgb_write(struct file *filp, const char *src_buf, size_t buf_cnt, loff_t* offset)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgb: write to device\n");
	#endif
	return 0;
}

int rgb_close(struct inode *inode, struct file *filp)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgb: close device\n");
	#endif
	return 0;
}

int rgb_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgb: ioctl\n");
	#endif
	return 0;
}

struct file_operations fops = {
	.owner =	THIS_MODULE,
	.open = 	rgb_open,
	.read = 	rgb_read,
	.write =	rgb_write,
	.release =	rgb_close,
	.ioctl =	rgb_ioctl
};

static int __init rgb_init(void)
{
	rgbdev.ret = alloc_chrdev_region(&rgbdev.dev_num, 0, 1, DEVICE_NAME);
	if (rgbdev.ret < 0) {
		prink(KERN_ALERT "rgb: allocating major num failed\n");
		return rgbdev.ret;
	}
	rgbdev.major_num = MAJOR(rgbdev.dev_num);
	#ifdef DEBUG
	printk(KERN_INFO "rgb: major num = %d\n", rgbdev.major_num);
	#endif

	rgbdev.cdev = cdev_alloc();
	rgbdev.cdev->ops = &fops;
	rgbdev.cdev->owner = THIS_MODULE;
	rgbdev.ret = cdev_add(rgbdev.cdev, rgbdev.dev_num, 1);
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "rgb: failed to add cdev\n");
		return rgbdev.ret;
	}
	// lock init
	rgbdev.dev_num = MKDEV(rgbdev.major_num, 0);
	// gpio configuration
	static struct gpio led_gpios[] = {
		{15, GPIOF_OUT_INIT_LOW, "Red"},
		{16, GPIOF_OUT_INIT_LOW, "Green"},
		{18, GPIOF_OUT_INIT_LOW, "Blue"},
		{22, GPIOF_OUT_INIT_LOW, "Clock"},
	};
	// Request GPIOs
	rgbdev.ret = gpio_request_array(led_gpios, ARRAY_SIZE(led_gpios));
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "gpio_request_array() error");
		return rgb.ret;
	}
	// Set GPIOs as output
	rgbdev.ret = gpio_direction_output(led_gpios[0], 0);
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error");
		return rgbdev.ret;
	}
	rgbdev.ret = gpio_direction_output(led_gpios[1], 0);
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error");
		return rgbdev.ret;
	}
	rgbdev.ret = gpio_direction_output(led_gpios[2], 0);
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error");
		return rgbdev.ret;
	}
	rgbdev.ret = gpio_direction_output(led_gpios[3], 0);
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error");
		return rgbdev.ret;
	}

	return 0;
}
