
#include <linux/kernel.h>
#include <linux/module.h>

void rt_prepare_poweroff(void)
{
	extern void extern_wm8960_reset(void);
	extern void mute_wm8960(void);
}

void mcu_power_off(void)
{

}



static int __init rt_init(void)
{
	return 0;
}


module_init(rt_init);
//module_exit(rt_exit);
