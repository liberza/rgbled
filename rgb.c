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

#define DRIVER_AUTHOR	"Nick Levesque <nick.levesque@gmail.com>"
#define DRIVER_DESC	"Sets red, green and blue values for external LED"
#define DEVICE_NAME	"rgb"
#define RGBIOCTL_MAGIC	0xB8
#define RGB_SET _IOW(RGBIOCTL_MAGIC, 1, colors_t *)

unsigned int red = 0;
unsigned int green = 0;
unsigned int blue = 0;

typedef struct {
	int red, green, blue;
} colors_t;

static struct class *class;

struct rgb_dev {
	int ret;
	dev_t dev_num;
	struct cdev *cdev;
	int major_num;
	struct mutex lock;
} rgbdev = {
	.major_num = 0,
	.ret = 0,
	.dev_num = 0,
};

static struct gpio led_gpios[] = {
	{22, GPIOF_OUT_INIT_LOW, "Red"},
	{23, GPIOF_OUT_INIT_LOW, "Green"},
	{24, GPIOF_OUT_INIT_LOW, "Blue"},
	{25, GPIOF_OUT_INIT_LOW, "Clock"},
};


// Implementation of file operation methods

int rgb_open(struct inode *inode, struct file *filp)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgb: opened device\n");
	#endif
	return 0;
}

int rgb_read(struct file *filp, char *buf, size_t buf_cnt, loff_t* offset)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgb read from device\n");
	#endif
	return -ENOTSUP;
}
 
ssize_t rgb_write(struct file *filp, const char *src_buf, size_t buf_cnt, loff_t* offset)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgb: write to device\n");
	#endif
	return -ENOTSUP;
}

int rgb_close(struct inode *inode, struct file *filp)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgb: close device\n");
	#endif
	return 0;
}

long rgb_ioctl(struct file *filp, unsigned int ioctl_num, unsigned long ioctl_param)
{
	int i=0;
	colors_t c;
	#ifdef DEBUG
	printk(KERN_INFO "rgb: ioctl\n");
	printk(KERN_INFO "ioctl_num: %d\n", ioctl_num);
	#endif
	switch (ioctl_num) {
		case RGB_SET:
			if (copy_from_user(&c, (colors_t *)ioctl_param, sizeof(colors_t))) {
				printk(KERN_INFO "rgb: copy_from_user failed\n");
				return -EACCES;
				break;
			}
			if ((c.red > 2047) | (c.green > 2047) | (c.blue > 2047)) {
				printk(KERN_INFO "rgb: invalid color value\n");
				return -EINVAL;
				break;
			}
			else if ((c.red < 0) | (c.green < 0) | (c.blue < 0)) {
				printk(KERN_INFO "rgb: invalid color value\n");
				return -EINVAL;
				break;
			}
			// wait for lock
			mutex_lock(&rgbdev.lock);
			
			red = c.red;
			green = c.green;
			blue = c.blue;
			// send 11 bits of RGB data
			for (i = 10; i >= 0; i--) {
				if (~(red >> i) & 1) 
					gpio_set_value(led_gpios[0].gpio, 1);
				if (~(green >> i) & 1) 
					gpio_set_value(led_gpios[1].gpio, 1);
				if (~(blue >> i) & 1) 
					gpio_set_value(led_gpios[2].gpio, 1);
				udelay(1);
				gpio_set_value(led_gpios[3].gpio, 1);
				udelay(4);
				gpio_set_value(led_gpios[0].gpio, 0);	
				gpio_set_value(led_gpios[1].gpio, 0);	
				gpio_set_value(led_gpios[2].gpio, 0);
				udelay(6);
				gpio_set_value(led_gpios[3].gpio, 0);
				udelay(10);
			}
			mutex_unlock(&rgbdev.lock);
			break;
		default:
			printk(KERN_INFO "rgb: invalid ioctl command\n");
			return -ENOTSUP;
	}

	return 0;
}

struct file_operations fops = {
	.owner =		THIS_MODULE,
	.open = 		rgb_open,
	.read = 		rgb_read,
	.write =		rgb_write,
	.release =		rgb_close,
	.unlocked_ioctl =	rgb_ioctl,
};

static int __init rgb_init(void)
{
	rgbdev.ret = alloc_chrdev_region(&rgbdev.dev_num, 0, 1, DEVICE_NAME);
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "rgb: allocating major num failed\n");
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

	// create device file
	if (IS_ERR(class = class_create(THIS_MODULE, "char"))) {
		cdev_del(rgbdev.cdev);
		unregister_chrdev_region(rgbdev.dev_num, 1);
		return -1;
	}

	if (IS_ERR(device_create(class, NULL, rgbdev.dev_num, NULL, "rgb"))) {
		class_destroy(class);
		cdev_del(rgbdev.cdev);
		unregister_chrdev_region(rgbdev.dev_num, 1);
		return -1;
	}
	// lock init
	mutex_init(&rgbdev.lock);

	// Request GPIOs
	rgbdev.ret = gpio_request_array(led_gpios, ARRAY_SIZE(led_gpios));
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "gpio_request_array() error\n");
		return rgbdev.ret;
	}
	// Set GPIOs as output
	rgbdev.ret = gpio_direction_output(led_gpios[0].gpio, 0);
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error\n");
		return rgbdev.ret;
	}
	rgbdev.ret = gpio_direction_output(led_gpios[1].gpio, 0);
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error\n");
		return rgbdev.ret;
	}
	rgbdev.ret = gpio_direction_output(led_gpios[2].gpio, 0);
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error\n");
		return rgbdev.ret;
	}
	rgbdev.ret = gpio_direction_output(led_gpios[3].gpio, 0);
	if (rgbdev.ret < 0) {
		printk(KERN_ALERT "gpio_direction_output() error\n");
		return rgbdev.ret;
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
	device_destroy(class, rgbdev.dev_num);
	#ifdef DEBUG
	printk(KERN_INFO "rgb: class_destroy\n");
	#endif
	class_destroy(class);
	unregister_chrdev_region(rgbdev.dev_num, 1);
	printk(KERN_ALERT "rgb: unloaded\n");
}

module_init(rgb_init);
module_exit(rgb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
