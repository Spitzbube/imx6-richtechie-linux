
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/power_supply.h>
#include <linux/earlysuspend.h>
#include <linux/reboot.h>	/* For kernel_power_off() */
#include <linux/richtechie/rt_battery.h>


#define	BATTERY_UPDATE_INTERVAL	100 /*seconds*/

extern atomic_t Data_c07c36a0; //c07c36a0

struct Struct_c0820d10
{
	int Data_0; //0
	int Data_4; //4
	int Data_8; //8
	int Data_12; //12
	struct i2c_client *i2c_client; //16
	int fill_20; //20
	struct power_supply	psy1; //24
	struct power_supply	psy2; //216
	struct work_struct work; //408
	struct workqueue_struct* Data_424; //424
	struct delayed_work delayed_work; //428
	struct rt_battery_platform_data* pdata; //472
};

static struct Struct_c0820d10* Data_c0820d10; //c0820d10
static int Data_c0820d20; //c0820d20 +16
static int Data_c0820d24; //c0820d24
static int Data_c0820d28[3]; //c0820d28
static int Data_c0820d40; //c0820d40
static int Data_c0820d44; //c0820d44 +52
static int Data_c0820d48; //c0820d48 +56


int my_i2c_read_bytes(struct i2c_client *client, u8 buf[], int len)
{
	int result;
	struct i2c_msg sp[2];

	sp[0].addr = client->addr;
	sp[0].flags = 0;
	sp[0].len = 1;
	sp[0].buf = buf;

	sp[1].addr = client->addr;
	sp[1].flags = I2C_M_RD;
	sp[1].len = len - 1;
	sp[1].buf = buf + 1;

	while (!atomic_dec_and_test(&Data_c07c36a0))
	{
		atomic_inc(&Data_c07c36a0);
	}
	msleep(5);

	result = i2c_transfer(client->adapter, &sp[0], 2);

	atomic_inc(&Data_c07c36a0);

	if (result <= 0)
	{
		printk(" i2c transfer error %d\015\012", result);
		msleep(600);
	}
	return result;
}

int my_i2c_write_bytes(struct i2c_client *client, u8 buf[], int len)
{
	int result;
	struct i2c_msg sp[2];

	sp[0].addr = client->addr;
	sp[0].flags = 0;
	sp[0].len = 1;
	sp[0].buf = buf;

	sp[1].addr = client->addr;
	sp[1].flags = 0;
	sp[1].len = len - 1;
	sp[1].buf = buf + 1;

	while (!atomic_dec_and_test(&Data_c07c36a0))
	{
		atomic_inc(&Data_c07c36a0);
	}
	msleep(5);

	result = i2c_transfer(client->adapter, &sp[0], 2);

	atomic_inc(&Data_c07c36a0);

	if (result <= 0)
	{
		printk(" i2c transfer error %d\015\012", result);
		msleep(600);
	}
	return result;
}

static void func_c04041b0(struct i2c_client *client)
{
	struct Struct_c0820d10* p = dev_get_drvdata(&client->dev);
	int r0;

	if (p->Data_0 != 0)
	{
		int r3 = p->Data_8 / 10;
		if (r3 < 356)
		{
			r0 = 0;
		}
		else if (r3 <=368)
		{
			r0 = (r3*100 - 35500) / 130;
		}
		else if (r3 <= 382)
		{
			r0 = (r3*100 - 36800) / 140 + 5;
		}
		else if (r3 <= 389)
		{
			r0 = (r3*100 - 38200) / 70 + 10;
		}
		else if (r3 <= 392)
		{
			r0 = (r3*100 - 38900) / 30 + 20;
		}
		else if (r3 < 396)
		{
			r0 = (r3*100 - 39200) / 30 + 30;
		}
		else if (r3 <= 397)
		{
			r0 = (r3*100 - 39500) / 20 + 40;
		}
		else if (r3 < 400)
		{
			r0 = (r3*100 - 39700) / 20 + 50;
		}
		else if (r3 <= 405)
		{
			r0 = (r3*100 - 39900) / 60 + 60;
		}
		else if (r3 <= 410)
		{
			r0 = (r3*100 - 40500) / 50 + 70;
		}
		else if (r3 <= 416)
		{
			r0 = (r3*100 - 41000) / 70 + 80;
		}
		else if (r3 <= 420)
		{
			r0 = (r3*100 - 41700) / 30 + 90;
		}
		else
		{
			r0 = 100;
		}
	}
	else
	{
		int r3 = p->Data_8 / 10;
		if (r3 <= 350)
		{
			r0 = 0;
		}
		else if (r3 <= 360)
		{
			r0 = (r3*100 - 35000) / 100;
		}
		else if (r3 <= 368)
		{
			r0 = (r3*100 - 36000) / 80 + 5;
		}
		else if (r3 <= 374)
		{
			r0 = (r3*100 - 36800) / 60 + 10;
		}
		else if (r3 <= 377)
		{
			r0 = (r3*100 - 37400) / 30 + 20;
		}
		else if (r3 < 380)
		{
			r0 = (r3*100 - 37700) / 20 + 30;
		}
		else if (r3 <= 382)
		{
			r0 = (r3*100 - 37900) / 30 + 40;
		}
		else if (r3 < 388)
		{
			r0 = (r3*100 - 38200) / 50 + 50;
		}
		else if (r3 <= 392)
		{
			r0 = (r3*100 - 38700) / 50 + 60;
		}
		else if (r3 <= 398)
		{
			r0 = (r3*100 - 39200) / 60 + 70;
		}
		else if (r3 <= 405)
		{
			r0 = (r3*100 - 39800) / 70 + 80;
		}
		else if (r3 < 416)
		{
			r0 = (r3*100 - 40500) / 100 + 85;
		}
		else
		{
			r0 = 100;
		}
	}

	if (Data_c0820d24 == 0)
	{
		//c0404204
		Data_c0820d40++;
		Data_c0820d28[0] = Data_c0820d28[1] = Data_c0820d28[2] = r0;
		Data_c0820d24 = 1;
	}
	else
	{
		//c0404314
		Data_c0820d28[Data_c0820d40] = r0;
		Data_c0820d40++;
		if (Data_c0820d40 > 2)
		{
			Data_c0820d40 = 0;
		}
	}
	//c040422c
	{
		int r0 = (Data_c0820d28[0] + Data_c0820d28[1] + Data_c0820d28[2]) / 3;
		if (r0 > 100) r0 = 100;
		if (r0 < 0) r0 = 0;
		p->Data_12 = r0;
	}
}

/*static*/ int GetMcuVoltage(struct i2c_client *client, int *pval)
{
	u8 buf[10];
	int val;

	memset(buf, 0, sizeof(buf));

	buf[0] = 0x11;

	if (my_i2c_read_bytes(client, buf, 4) <= 0)
	{
		msleep(200);
		return 1;
	}

	if ((buf[1] ^ buf[2]) != buf[3])
	{
		msleep(200);
		return 2;
	}

	val = buf[1] + (buf[2] << 8);
	*pval = val + 6;
	val = (val * 6600) >> 10;

	if (val > 4260)
	{
		msleep(200);
		return 3;
	}

	if (val <= 99)
	{
		msleep(200);
		return 4;
	}

	return 0;
}

void rt_get_vcell(struct i2c_client *client)
{
	int sum = 0;
	int sp4 = 0;
	int i = 0;
	int min = 0;
	int max = 0;
	struct Struct_c0820d10* p = dev_get_drvdata(&client->dev);

	while (i < 5)
	{
		if (GetMcuVoltage(client, &sp4))
		{
			msleep(2000);
		}
		else
		{
			int val = sp4 * 6600 / 1024;
			if (i == 0)
			{
				min = max = val;
			}
			else
			{
				if (val < min) min = val;
				if (val > max) max = val;
			}
			sum += val;
			i++;
		}

		msleep(300);
	}

	p->Data_8 = (sum - min - max) / 3;
}

void bat_radm(struct timeval *ptv)
{
	static union
	{
		struct timeval tv;
		u8 array[sizeof(struct timeval)];
	} tv;
	static u8 count;
	u8* pb = (void*) ptv;

	do_gettimeofday(&tv.tv);

	pb[0] = tv.tv.tv_usec;
	pb[1] = tv.array[5];
	pb[2] = tv.array[6];
	pb[3] = tv.array[7];

	pb[4] = tv.tv.tv_sec;
	pb[5] = tv.array[1];
	pb[6] = tv.array[2];
	pb[7] = tv.array[3];

	pb[1] += (pb[4] + count++);
	pb[2] += pb[5];
	pb[3] += (pb[6] + count++);
	pb[4] += pb[7];
}

int checkMyBoard(struct i2c_client *client)
{
	u8 buf[8];
	struct timeval tv;

	bat_radm(&tv);
	buf[0] = 0x12;
	memcpy(&buf[1], &tv.tv_sec, sizeof(__kernel_time_t));
	buf[5] = buf[1] ^ buf[2] ^ buf[3] ^ buf[4];

	if (my_i2c_write_bytes(client, buf, 6) <= 0)
	{
		msleep(200);
		return 1;
	}

	msleep(100);

	buf[0] = 0x14;
	if (my_i2c_read_bytes(client, buf, 6) <= 0)
	{
		msleep(200);
		return 2;
	}

	if (buf[5] != (buf[1] ^ buf[2] ^ buf[3] ^ buf[4]))
	{
		msleep(200);
		return 4;
	}

	buf[1] ^= 0x86;
	buf[2] ^= 0x11;
	buf[3] ^= 0x04;
	buf[4] ^= 0x73;

	if (memcmp(&buf[1], &tv.tv_sec, sizeof(__kernel_time_t)) != 0)
	{
		msleep(200);
		return 3;
	}

	return 0;
}

static void GetMcuVersion(struct i2c_client *client)
{
	u8 buf[10];

	memset(buf, 0, sizeof(buf));

	buf[0] = 0x13;
	buf[1] = 0xAB;

	if (my_i2c_write_bytes(client, buf, 2) <= 0)
	{
		printk("------------------get mcu version error\015\012");
	}
}

/* todo */
void rt_get_status(struct i2c_client *client)
{
	struct Struct_c0820d10* p = dev_get_drvdata(&client->dev);
	struct rt_battery_platform_data* pdata = p->pdata;

	if (pdata->Data_4 && pdata->Func_0)
	{
		if ((pdata->Func_0)() != 0)
		{
			if (p->Data_12 <= 96)
			{
				p->Data_4 = 1;
			}
			else
			{
				p->Data_4 = 3;
			}
		}
		else
		{
			p->Data_4 = 2;
		}

		if (p->Data_12 > 96)
		{
			p->Data_4 = 4;
		}
	}
	else
	{
		p->Data_4 = 0;
	}
}

void rt_get_online(struct i2c_client *client)
{
	struct Struct_c0820d10* p = dev_get_drvdata(&client->dev);
	struct rt_battery_platform_data* pdata = p->pdata;

	if (pdata->Func_0)
	{
		p->Data_0 = (pdata->Func_0)();
	}
	else
	{
		p->Data_0 = 1;
	}
}

void rt_charger_update_status(struct Struct_c0820d10* p)
{
	rt_get_status(p->i2c_client);
	power_supply_changed(&p->psy1);
	rt_get_online(p->i2c_client);
	power_supply_changed(&p->psy2);
}

static int rt_batt_suspend(struct device *dev)
{
	struct Struct_c0820d10* p = dev_get_drvdata(&Data_c0820d10->i2c_client->dev);

	cancel_delayed_work(&p->delayed_work);

	return 0;
}

static int rt_batt_resume(struct device *dev)
{
	struct Struct_c0820d10* p = dev_get_drvdata(&Data_c0820d10->i2c_client->dev);

	schedule_delayed_work(&p->delayed_work, 600);

	rt_charger_update_status(Data_c0820d10);

	return 0;
}

static int rt_batt_get_property(struct power_supply *psy,
		    enum power_supply_property psp,
		    union power_supply_propval *val)
{
	int result = 0;
	struct Struct_c0820d10* p = container_of(psy, struct Struct_c0820d10, psy1);

	//printk("rt_batt_get_property: psp = %d\n", psp);

	switch (psp)
	{
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = p->Data_4;
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = 2;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = p->Data_8;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = p->Data_12;
		break;
	default:
		result = -22;
		break;
	}

	return result;
}

static int rt_ac_get_property(struct power_supply *psy,
		    enum power_supply_property psp,
		    union power_supply_propval *val)
{
	int result = 0;
	struct Struct_c0820d10* p = container_of(psy, struct Struct_c0820d10, psy2);

	//printk("rt_ac_get_property: psp = %d\n", psp);

	if (psp == POWER_SUPPLY_PROP_ONLINE)
	{
		val->intval = p->Data_0;
	}

	return result;
}

static irqreturn_t rt_charger_switch_irq(int a, void* b)
{
	struct Struct_c0820d10* p = (struct Struct_c0820d10*) b;

//	printk("rt_charger_switch_irq: a = %d\n", a);

	disable_irq_nosync(p->i2c_client->irq);

	queue_work(p->Data_424, &p->work);

	return 1;
}

static void rt_charger_work(struct work_struct *work)
{
	struct Struct_c0820d10* p = container_of(work, struct Struct_c0820d10, work);

	rt_charger_update_status(p);

	enable_irq(p->i2c_client->irq);
}

static void rt_batt_work(struct work_struct *work)
{
	struct Struct_c0820d10* p = container_of(work, struct Struct_c0820d10, delayed_work.work);
	int r6 = 3530;

	rt_get_online(p->i2c_client);

	power_supply_changed(&p->psy2);

	rt_get_vcell(p->i2c_client);

	if ((p->Data_8 <= r6) && (p->Data_0 == 0))
	{
		//c040411c
		for (Data_c0820d20 = 0; Data_c0820d20 <= 2; Data_c0820d20++)
		{
			//->c040413c
			rt_get_vcell(p->i2c_client);

			if (p->Data_8 > r6)
			{
				break;
			}
		}
		//c040415c
		if (Data_c0820d20 > 2)
		{
			printk("battery low -> power_off\015\012");
			kernel_power_off();

			GetMcuVersion(p->i2c_client);
		}
		//->c04041b4
	}
	//c04041b0
	func_c04041b0(p->i2c_client);

	rt_get_status(p->i2c_client);
	power_supply_changed(&p->psy1);

	Data_c0820d44++;
	if ((Data_c0820d44 % 10) == 0)
	{
		//c0404388
		if (checkMyBoard(p->i2c_client) != 0)
		{
			//c040439c
			Data_c0820d48++;
			if (Data_c0820d48 > 9)
			{
				//c04043b0
				printk("check board err\015\012");
				kernel_power_off();
				while (1) {}
			}
			//->c0404294
		}
		else
		{
			Data_c0820d48 = 0;
			//->c0404294
		}
	}
	//c0404294
	if (p->Data_12 <= 9)
	{
		//->c04043c0
		schedule_delayed_work(&p->delayed_work, 200);
	}
	else if (p->Data_12 <= 19)
	{
		schedule_delayed_work(&p->delayed_work, 300);
	}
	else
	{
		schedule_delayed_work(&p->delayed_work, 600);
	}
}

static void rt_battery_early_suspend(struct early_suspend *h)
{
	/* complete */
}

static void rt_battery_late_resume(struct early_suspend *h)
{
	/* complete */
}

static enum power_supply_property power_props1[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TECHNOLOGY,
};

static enum power_supply_property power_props2[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static struct early_suspend rt_batt_early_suspend_handler = {
	.level = EARLY_SUSPEND_LEVEL_STOP_DRAWING,
	.suspend = rt_battery_early_suspend,
	.resume = rt_battery_late_resume,
};


static int __devinit battery_i2c_probe(struct i2c_client *client,
				   const struct i2c_device_id id)
{
	int result = 0;
	struct i2c_adapter *adapter;
	struct Struct_c0820d10* p;
	struct rt_battery_platform_data* pdata;

	printk("rt_battery probe ");

	adapter = to_i2c_adapter(client->dev.parent);

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE))
	{
		return -5;
	}

	p = kzalloc(sizeof(struct Struct_c0820d10), GFP_KERNEL);
	if (p == NULL)
	{
		return -ENOMEM;
	}

	p->i2c_client = client;

	dev_set_drvdata(&client->dev, p);

	p->pdata = client->dev.platform_data;
	p->pdata->Func_12();

	p->psy1.name = "rt-battery";
	p->psy1.type = POWER_SUPPLY_TYPE_BATTERY;
	p->psy1.properties		= power_props1;
	p->psy1.num_properties	= ARRAY_SIZE(power_props1);
	p->psy1.get_property	= rt_batt_get_property;

	p->psy2.name = "rt-ac";
	p->psy2.type = POWER_SUPPLY_TYPE_MAINS;
	p->psy2.properties		= power_props2;
	p->psy2.num_properties	= ARRAY_SIZE(power_props2);
	p->psy2.get_property	= rt_ac_get_property;

	Data_c0820d10 = p;

	result = power_supply_register(&client->dev, &p->psy1);
	if (result)
	{
		dev_err(&client->dev, "failed: rt_battery power supply register\n");
		kfree(p);
		return result;
	}

	result = power_supply_register(&client->dev, &p->psy2);
	if (result)
	{
		dev_err(&client->dev, "failed: rt_ac power supply register\n");
		kfree(p);
		return result;
	}

	pdata = p->pdata;
	result = request_threaded_irq(pdata->irq,
			rt_charger_switch_irq, NULL,
			IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
			client->name, p);

	p->Data_424 = create_workqueue("ac_wq");
	if (p->Data_424 == NULL)
	{
		//result = -ENOMEM;
		printk("<1>creat workqueue faiked\n");
		return -ENOMEM;
	}

	INIT_WORK(&p->work, rt_charger_work);
	INIT_DELAYED_WORK_DEFERRABLE(&p->delayed_work, rt_batt_work);

	schedule_delayed_work(&p->delayed_work, BATTERY_UPDATE_INTERVAL);

	register_early_suspend(&rt_batt_early_suspend_handler);

	printk("end\015\012");

	return 0;
}

static int __devexit battery_i2c_remove(struct i2c_client *client)
{
	struct Struct_c0820d10* p = dev_get_drvdata(&client->dev);

	power_supply_unregister(&p->psy1);
	power_supply_unregister(&p->psy2);

	cancel_delayed_work(&p->delayed_work);

	kfree(p);

	return 0;
}

static const struct dev_pm_ops rt_battery_dev_pm_ops =
{
	.suspend = rt_batt_suspend,
	.resume = rt_batt_resume,
	.freeze = rt_batt_suspend,
	.thaw = rt_batt_resume,
	.poweroff = rt_batt_suspend,
	.restore = rt_batt_resume,
};

static const struct i2c_device_id battery_id[] = {
	{"rt_battery-i2c", 0},
	{},
};

static struct i2c_driver battery_driver = //c07ecba0
{
	.driver = {
		   .name = "rt_battery-i2c",
		   .owner = THIS_MODULE,
		   .pm = &rt_battery_dev_pm_ops,
		   },
	.probe = battery_i2c_probe,
	.remove = __devexit_p(battery_i2c_remove),
	.id_table = battery_id,
};


/*static*/ int battery_i2c_init(void)
{
	return i2c_add_driver(&battery_driver);
}

/*static*/ void battery_i2c_exit(void)
{
	i2c_del_driver(&battery_driver);
}

static int __init android_bat_init(void)
{
	int ret;

	printk("<6>Android Battery driver loading...\015\012");

	ret = battery_i2c_init();

	return ret;
}


static void __exit android_bat_exit(void)
{
	battery_i2c_exit();
}

module_init(android_bat_init);
module_exit(android_bat_exit);
