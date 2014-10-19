
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>


static struct i2c_driver battery_driver = //c07ecba0
{
#if 0
	.driver = {
		   .name = MMA8452_DRV_NAME,
		   .owner = THIS_MODULE,
		   },
	.suspend = mma8452_suspend,
	.resume = mma8452_resume,
	.probe = mma8452_probe,
	.remove = __devexit_p(mma8452_remove),
	.id_table = mma8452_id,
#endif
};


static int battery_i2c_init(void)
{
	return i2c_add_driver(&battery_driver);
}

static void battery_i2c_exit(void)
{
	i2c_del_driver(&battery_driver);
}

static int __init android_bat_init(void)
{
	int ret;

	printk("todo");

	ret = battery_i2c_init();

	return ret;
}


static void __exit android_bat_exit(void)
{
	battery_i2c_exit();
}

module_init(android_bat_init);
module_exit(android_bat_exit);
