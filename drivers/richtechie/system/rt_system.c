
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

void rt_prepare_poweroff(void)
{
	extern void extern_wm8960_reset(void);
	extern void mute_wm8960(void);
}

void mcu_power_off(void)
{
}

struct Struct_c0820d6c
{
	int Data_0; //0
	struct semaphore sem; //4
	struct cdev cdev; //24
};

struct Struct_c0820d6c_Outer
{
	struct Struct_c0820d6c *Data_c0820d6c; //c0820d6c
	int major; //c0820d6c +4
	int minor; //c0820d6c +8
	struct class *rt_class; //c0820d6c +12
};

static struct Struct_c0820d6c_Outer Data_c0820d6c; //c0820d6c

static int rt_open(struct inode *, struct file *);
static int rt_release(struct inode *, struct file *);
static ssize_t rt_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t rt_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t rt_val_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t rt_val_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

static const struct file_operations rt_fops = //c07ece28
{
		.open = rt_open,
		.release = rt_release,
		.read = rt_read,
		.write = rt_write,
};
#if 0
static const struct device_attribute dev_attr_val = //c07ece28 +104
{};
#else
const DEVICE_ATTR(val, 0644, rt_val_show, rt_val_store);
#endif

extern void setGsensorException(int);

static int __rt_get_val(struct Struct_c0820d6c* a, char* b)
{
	int ret = 0;
	struct semaphore *sem = &a->sem;

	if (down_interruptible(sem))
	{
		ret = -0x200;
	}
	else
	{
		int val = a->Data_0;
		up(sem);
		ret = snprintf(b, 0x1000, "%d/n", val);
	}

	return ret;
}

static int __rt_set_val(struct Struct_c0820d6c* a, const char* b, int c)
{
	int ret = 0;
	struct semaphore *sem = &a->sem;
	int val = simple_strtol(b, 0, 10);

	if (down_interruptible(sem))
	{
		ret = -0x200;
	}
	else
	{
		a->Data_0 = val;
		setGsensorException(val);
		up(sem);
		ret = c;
	}
	return ret;
}

static int rt_open(struct inode *r0, struct file *r1)
{
	r1->private_data = (char*)(r0->i_cdev) - 24;
	return 0;
}

static int rt_release(struct inode *r0, struct file *r1)
{
	return 0;
}

static ssize_t rt_read(struct file *file, char __user *user_buf, size_t count, loff_t *offset)
{
	ssize_t ret = 0;
	struct Struct_c0820d6c* priv = (struct Struct_c0820d6c*) file->private_data;
	struct semaphore *sem = &priv->sem;

	if (down_interruptible(sem))
	{
		ret = -0x200;
	}
	else
	{
		if (count > 3)
		{
			ret = copy_to_user(user_buf, &priv->Data_0, 4)? -14: 4;
		}

		up(sem);
	}

	return ret;
}

static ssize_t rt_write(struct file *file, const char __user *user_buf, size_t count, loff_t *offset)
{
	ssize_t ret = 0;
	struct Struct_c0820d6c* priv = (struct Struct_c0820d6c*) file->private_data;
	struct semaphore *sem = &priv->sem;

	if (down_interruptible(sem))
	{
		ret = -0x200;
	}
	else
	{
		if (count == 4)
		{
			if (0 == copy_from_user(&priv->Data_0, user_buf, count))
			{
				setGsensorException(priv->Data_0);
				ret = count;
			}
			else
			{
				ret = -14;
			}
		}
		up(sem);
	}

	return ret;
}

static ssize_t rt_val_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct Struct_c0820d6c* r0 = dev_get_drvdata(dev);

	return __rt_get_val(r0, buf);
}

static ssize_t rt_val_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct Struct_c0820d6c* r0 = dev_get_drvdata(dev);

	return __rt_set_val(r0, buf, count);
}

static int rt_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int ret = 0;
	if (off > 0)
	{
		*eof = 1;
	}
	else
	{
		ret = __rt_get_val(Data_c0820d6c.Data_c0820d6c, page);
	}
	return ret;
}

static int rt_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	int ret = 0;
	void* addr;

	if (count > 0x1000)
	{
		printk("<1>The buff is too large: %lu./n", count);
		return -14;
	}

	addr = (void*) __get_free_pages(___GFP_WAIT | ___GFP_IO | ___GFP_FS, 0);
	if (addr == 0)
	{
		printk("<1>Failed to alloc page./n");
		return -12;
	}

	if (0 != copy_from_user(addr, buffer, count))
	{
		printk("<1>Failed to copy buff from user./n");
		ret = -14;
	}
	else
	{
		ret = __rt_set_val(Data_c0820d6c.Data_c0820d6c, addr, count);
	}

	free_pages((unsigned long) addr, 0);

	return ret;
}


static int rt_init_inner1(struct Struct_c0820d6c* sl, dev_t r9, const struct file_operations* fops)
{
	int ret;
	struct cdev *r5 = &sl->cdev;

	memset(sl, 0, sizeof(struct Struct_c0820d6c));

	cdev_init(r5, fops);

	r5->owner = 0;
	r5->ops = fops;

	ret = cdev_add(r5, r9, 1);
	if (ret != 0)
	{
		return ret;
	}

	sema_init(&sl->sem, 1);
	sl->Data_0 = 0;

	return 0;
}

static int __init rt_init(void)
{
	int ret;
	char* r6 = "rtSystem";
	dev_t dev = 0;
	struct device  *r7_r8;
	struct Struct_c0820d6c* sl;
	struct class *r3;
	struct proc_dir_entry* proc_attr;

	printk("<1>Initializing rt device./n");

	ret = alloc_chrdev_region(&dev, 0, 1, r6);
	if (ret < 0)
	{
		printk("<1>Failed to alloc char dev region./n");
		return ret;
	}

	Data_c0820d6c.major = MAJOR(dev);
	Data_c0820d6c.minor = MINOR(dev);

	Data_c0820d6c.Data_c0820d6c = sl = kmalloc(sizeof(struct Struct_c0820d6c), GFP_KERNEL);
	if (Data_c0820d6c.Data_c0820d6c == NULL)
	{
		printk("<1>Failed to alloc rt_dev./n");
		ret = -ENOMEM;
		//->c0024604
		goto err4;
	}

	ret = rt_init_inner1(sl, MKDEV(Data_c0820d6c.major, Data_c0820d6c.minor), &rt_fops);
	if (ret != 0)
	{
		//c0024524
		printk("<1>Failed to setup dev: %d./n", ret);
		//->c00245f8
		goto err3;
	}

	Data_c0820d6c.rt_class = r3 = __class_create(THIS_MODULE, r6, &Data_c0820d6c.rt_class);/*class_create(THIS_MODULE, r6);*/
	if (IS_ERR(r3))
	{
		printk("<1>Failed to create rt class./n");
		ret = PTR_ERR(r3);
		//->c00245e8
		goto err2;
	}

	r7_r8 = device_create(Data_c0820d6c.rt_class, NULL, dev, "%s", r6);
	if (IS_ERR(r7_r8))
	{
		printk("<1>Failed to create rt device.");
		//->c00245d8
		ret = PTR_ERR(r7_r8);
		goto err1;
	}

	ret = device_create_file(r7_r8, &dev_attr_val);
	if (ret < 0)
	{
		//c0024584
		printk("<1>Failed to create attribute val.");
		device_destroy(Data_c0820d6c.rt_class, dev);
		//->c00245d8
		goto err1;
	}

	dev_set_drvdata(r7_r8, Data_c0820d6c.Data_c0820d6c);

	proc_attr = create_proc_entry(r6, 0, 0);
				      /*S_IFREG | S_IRUGO |
				      S_IWUSR, g_asrc->proc_asrc);*/
	if (proc_attr) {
		proc_attr->read_proc = rt_proc_read;
		proc_attr->write_proc = rt_proc_write;
	}

	printk("<1>Succedded to initialize rt device./n");

	return 0;

err1:
	//c00245d8
	class_destroy(Data_c0820d6c.rt_class);
err2:
	//c00245e8
	cdev_del(&Data_c0820d6c.Data_c0820d6c->cdev);
err3:
	//c00245f8
	kfree(Data_c0820d6c.Data_c0820d6c);
err4:
	//c0024604
	unregister_chrdev_region(MKDEV(Data_c0820d6c.major, Data_c0820d6c.minor), 1);

	return ret;
}


static void __exit rt_exit(void)
{
	dev_t devt = MKDEV(Data_c0820d6c.major, Data_c0820d6c.minor);

	printk("<1>Destroy rt device./n");

	remove_proc_entry("rtSystem", 0);

	if (Data_c0820d6c.rt_class)
	{
		device_destroy(Data_c0820d6c.rt_class, MKDEV(Data_c0820d6c.major, Data_c0820d6c.minor));
		class_destroy(Data_c0820d6c.rt_class);
	}

	if (Data_c0820d6c.Data_c0820d6c)
	{
		cdev_del(&Data_c0820d6c.Data_c0820d6c->cdev);
		kfree(Data_c0820d6c.Data_c0820d6c);
	}

	unregister_chrdev_region(devt, 1);
}


module_init(rt_init);
module_exit(rt_exit);
