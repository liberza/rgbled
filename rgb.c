#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/ioctl.h>
#include <linux/device.h>
#include <linux/stat.h>

#define DRIVER_AUTHOR	"Nick Levesque <nick.levesque@gmail.com>"
#define DRIVER_DESC		"Sets red, green and blue values for external LED"
#define DEVICE_NAME		"rgb"
#define RGBIOCTL_MAGIC	0xB8
#define RED 			22
#define GREEN 			23
#define BLUE 			24
#define CLK  			25
#define RGB_SET _IOW(RGBIOCTL_MAGIC, 1, colors_t *)

// struct for getting color data from ioctl
typedef struct {
	unsigned int red, green, blue;
} colors_t;

// rgb_dev struct for keeping global variables to a minimum
struct rgb_dev {
	dev_t dev_num;
	struct cdev *cdev;
	int major_num;
	struct mutex lock;
	struct class *class;
} rgbdev = {
	.major_num = 0,
	.dev_num = 0,
};

// array of gpio structs to be used in gpio_request_array()
static struct gpio led_gpios[] = {
	{RED, GPIOF_OUT_INIT_LOW, "Red"},
	{GREEN, GPIOF_OUT_INIT_LOW, "Green"},
	{BLUE, GPIOF_OUT_INIT_LOW, "Blue"},
	{CLK, GPIOF_OUT_INIT_LOW, "Clock"},
};


// Implementation of file operation methods

int rgb_open(struct inode *inode, struct file *filp)
{
		// Only opening as write-only is permitted
        if ((filp->f_flags&O_ACCMODE)==O_RDONLY) return -EPERM;
        if ((filp->f_flags&O_ACCMODE)==O_RDWR) return -EPERM;
	return 0;
}

// Should never get called, if permissions on device file are correct
// but if it does, tell the user "operation not permitted" 
int rgb_read(struct file *filp, char *buf, size_t buf_cnt, loff_t* offset)
{
	return -EPERM;
}
 
// Not permitted, must pass data via ioctl
ssize_t rgb_write(struct file *filp, const char *src_buf, size_t buf_cnt, loff_t* offset)
{
	return -EPERM;
}

int rgb_close(struct inode *inode, struct file *filp)
{
	return 0;
}

// Get data from user, then send color bits via GPIO
long rgb_ioctl(struct file *filp, unsigned int ioctl_num, unsigned long ioctl_param)
{
	int i=0;
	colors_t c;
	switch (ioctl_num) {
		case RGB_SET:
			mutex_lock(&rgbdev.lock);
			// get RGB data from user
			if (copy_from_user(&c, (colors_t *)ioctl_param, sizeof(colors_t))) {
				printk(KERN_INFO "rgb: copy_from_user failed\n");
				mutex_unlock(&rgbdev.lock);
				return -EFAULT;
				break;
			}
			// check that passed data is valid
			if ((c.red > 2047) | (c.green > 2047) | (c.blue > 2047)) {
				printk(KERN_INFO "rgb: invalid color value\n");
				mutex_unlock(&rgbdev.lock);
				return -EINVAL;
				break;
			}
			else if ((c.red < 0) | (c.green < 0) | (c.blue < 0)) {
				printk(KERN_INFO "rgb: invalid color value\n");
				mutex_unlock(&rgbdev.lock);
				return -EINVAL;
				break;
			}
			// send bits over GPIO
			for (i = 10; i >= 0; i--) {
				if (~(c.red >> i) & 1) 
					gpio_set_value(RED, 1);
				if (~(c.green >> i) & 1) 
					gpio_set_value(GREEN, 1);
				if (~(c.blue >> i) & 1) 
					gpio_set_value(BLUE, 1);
				udelay(1);
				gpio_set_value(CLK, 1);
				udelay(4);
				gpio_set_value(RED, 0);	
				gpio_set_value(GREEN, 0);	
				gpio_set_value(BLUE, 0);
				udelay(6);
				gpio_set_value(CLK, 0);
				udelay(10);
			}
			mutex_unlock(&rgbdev.lock);
			break;
		default:
			printk(KERN_INFO "rgb: invalid ioctl command\n");
			return -ENOTTY;
	}

	return 0;
}

struct file_operations rgbfops = {
	.owner =                THIS_MODULE,
	.open =                 rgb_open,
	.read =                 rgb_read,
	.write =                rgb_write,
	.release =              rgb_close,
	.unlocked_ioctl =       rgb_ioctl,
};

static int __init rgb_init(void)
{
	int ret;
	ret = alloc_chrdev_region(&rgbdev.dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		printk(KERN_ALERT "rgb: allocating major num failed\n");
		return ret;
	}

	rgbdev.major_num = MAJOR(rgbdev.dev_num);
	#ifdef DEBUG
	printk(KERN_INFO "rgb: major num = %d\n", rgbdev.major_num);
	#endif

	rgbdev.cdev = cdev_alloc();
	rgbdev.cdev->ops = &rgbfops;
	rgbdev.cdev->owner = THIS_MODULE;
	ret = cdev_add(rgbdev.cdev, rgbdev.dev_num, 1);
	if (ret < 0) {
		printk(KERN_ALERT "rgb: failed to add cdev\n");
		return ret;
	}

	// create device class
	if (IS_ERR(rgbdev.class = class_create(THIS_MODULE, "char"))) {
		cdev_del(rgbdev.cdev);
		unregister_chrdev_region(rgbdev.dev_num, 1);
		return -1;
	}
	// create device file
	if (IS_ERR(device_create(rgbdev.class, NULL, rgbdev.dev_num, NULL, "rgb"))) {
		class_destroy(rgbdev.class);
		cdev_del(rgbdev.cdev);
		unregister_chrdev_region(rgbdev.dev_num, 1);
		return -1;
	}
	// lock init
	mutex_init(&rgbdev.lock);

	// request GPIOs
	ret = gpio_request_array(led_gpios, ARRAY_SIZE(led_gpios));
	if (ret < 0) {
		printk(KERN_ALERT "gpio_request_array() error\n");
		return ret;
	}
	// set GPIOs as output
	ret = gpio_direction_output(led_gpios[0].gpio, 0);
	if (ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error\n");
		return ret;
	}
	ret = gpio_direction_output(led_gpios[1].gpio, 0);
	if (ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error\n");
		return ret;
	}
	ret = gpio_direction_output(led_gpios[2].gpio, 0);
	if (ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error\n");
		return ret;
	}
	ret = gpio_direction_output(led_gpios[3].gpio, 0);
	if (ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error\n");
		return ret;
	}

	return 0;
}

static void __exit rgb_exit(void)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgb: deleting cdev\n"); 
	#endif
	cdev_del(rgbdev.cdev);
	#ifdef DEBUG
	printk(KERN_INFO "rgb: gpio_free_array\n"); 
	#endif
	gpio_free_array(led_gpios, ARRAY_SIZE(led_gpios));
	#ifdef DEBUG
	printk(KERN_INFO "rgb: device_destroy\n");
	#endif
	device_destroy(rgbdev.class, rgbdev.dev_num);
	#ifdef DEBUG
	printk(KERN_INFO "rgb: class_destroy\n");
	#endif
	class_destroy(rgbdev.class);
	unregister_chrdev_region(rgbdev.dev_num, 1);
	printk(KERN_ALERT "rgb: unloaded\n");
}

module_init(rgb_init);
module_exit(rgb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
