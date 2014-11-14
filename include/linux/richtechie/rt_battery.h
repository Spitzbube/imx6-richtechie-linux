
#ifndef __RT_BATTERY_H_
#define __RT_BATTERY_H_

struct rt_battery_platform_data //->See Data_c07c371c
{
	int (*Func_0)(void); //0
	int Data_4; //4
	int fill_8; //8
	void (*Func_12)(void); //12
	unsigned int irq; //16
};

#endif /* __RT_BATTERY_H_ */
