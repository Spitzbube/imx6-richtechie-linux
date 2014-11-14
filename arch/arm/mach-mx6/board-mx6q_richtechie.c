/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/nodemask.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/fsl_devices.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/i2c.h>
#include <linux/i2c/pca953x.h>
#include <linux/ata.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/regulator/consumer.h>
#include <linux/pmic_external.h>
#include <linux/pmic_status.h>
#include <linux/ipu.h>
#include <linux/mxcfb.h>
#include <linux/pwm_backlight.h>
#include <linux/fec.h>
#include <linux/memblock.h>
#include <linux/gpio.h>
#include <linux/ion.h>
#include <linux/etherdevice.h>
#include <linux/regulator/anatop-regulator.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>
#include <linux/mfd/mxc-hdmi-core.h>
#include <linux/richtechie/rt_battery.h>

#include <mach/common.h>
#include <mach/hardware.h>
#include <mach/mxc_dvfs.h>
#include <mach/memory.h>
#include <mach/iomux-mx6q.h>
#include <mach/imx-uart.h>
#include <mach/viv_gpu.h>
#include <mach/ipu-v3.h>
#include <mach/mxc_hdmi.h>

#include <asm/irq.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>

#ifdef CONFIG_IMX_PCIE
#include <linux/wakelock.h>
#endif

#ifdef  CONFIG_MFD_WM831X
#include <linux/mfd/wm831x/core.h>
#include <linux/mfd/wm831x/pdata.h>
#include <linux/mfd/wm831x/regulator.h>
#include <linux/mfd/wm831x/gpio.h>
#endif

#include "usb.h"
#include "devices-imx6q.h"
#include "crm_regs.h"
#include "cpu_op-mx6.h"
#include "board-mx6dl_richtechie.h"
#include "board-mx6q_richtechie.h"

//#define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))
//#define SABRESD_USB_H1_PWR	IMX_GPIO_NR(1, 29)
#define RICHTECHIE_LCD_EN	IMX_GPIO_NR(1, 8)
#define RICHTECHIE_AC_CHARGING	IMX_GPIO_NR(1, 11)
#define RICHTECHIE_LDB_PWR	IMX_GPIO_NR(2, 4)
#define RICHTECHIE_LDB_BACKLIGHT	IMX_GPIO_NR(2, 5)
#define RICHTECHIE_AC_DET			IMX_GPIO_NR(2, 6)
#define RICHTECHIE_USB_HOST_PWR_EN	IMX_GPIO_NR(2, 7)
#define RICHTECHIE_VOLUME_DN	IMX_GPIO_NR(3, 16)
#define RICHTECHIE_POWER_KEY	IMX_GPIO_NR(3, 18)
#define RICHTECHIE_BACK		IMX_GPIO_NR(3, 19)
#define RICHTECHIE_VOLUME_UP	IMX_GPIO_NR(3, 20)
#define RICHTECHIE_USB_OTG_PWR	IMX_GPIO_NR(3, 22)
#define RICHTECHIE_SPEAKER_EN	IMX_GPIO_NR(3, 31)
#define RICHTECHIE_SENSOR_INT1	IMX_GPIO_NR(4, 14)
#define RICHTECHIE_SENSOR_INT2	IMX_GPIO_NR(4, 15)
#define RICHTECHIE_TEST_1	IMX_GPIO_NR(5, 22)
#define RICHTECHIE_TEST_2	IMX_GPIO_NR(5, 23)
#define RICHTECHIE_TEST_3	IMX_GPIO_NR(5, 25)
#define RICHTECHIE_HEADPHONE_DET	IMX_GPIO_NR(6, 7)
#define RICHTECHIE_TS_INT		IMX_GPIO_NR(6, 9)
#define RICHTECHIE_SD3_CD		IMX_GPIO_NR(6, 11)
#define RICHTECHIE_POWER_OFF	IMX_GPIO_NR(7, 13)


extern char *gp_reg_id; //c07fd4f0
extern char *soc_reg_id; //c07fd504
extern char *pu_reg_id; //c07fd50c

int Data_c07fd670;


static iomux_v3_cfg_t mx6q_sd3_50mhz[] = {
MX6Q_PAD_SD3_CLK__USDHC3_CLK_50MHZ, //IOMUX_PAD(0x06a4, 0x02bc, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 0 000 6a4 2bc,
MX6Q_PAD_SD3_CMD__USDHC3_CMD_50MHZ, //IOMUX_PAD(0x06a0, 0x02b8, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b3 0 000 6a0 2b8,
MX6Q_PAD_SD3_DAT0__USDHC3_DAT0_50MHZ, //IOMUX_PAD(0x06a8, 0x02c0, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 0 000 6a8 2c0,
MX6Q_PAD_SD3_DAT1__USDHC3_DAT1_50MHZ, //IOMUX_PAD(0x06ac, 0x02c4, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 0 000 6ac 2c4,
MX6Q_PAD_SD3_DAT2__USDHC3_DAT2_50MHZ, //IOMUX_PAD(0x06b0, 0x02c8, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 0 000 6b0 2c8,
MX6Q_PAD_SD3_DAT3__USDHC3_DAT3_50MHZ, //IOMUX_PAD(0x06b4, 0x02cc, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 0 000 6b4 2cc,
MX6Q_PAD_SD3_DAT4__USDHC3_DAT4_50MHZ, //IOMUX_PAD(0x069c, 0x02b4, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 0 000 69c 2b4,
MX6Q_PAD_SD3_DAT5__USDHC3_DAT5_50MHZ, //IOMUX_PAD(0x0698, 0x02b0, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 0 000 698 2b0,
MX6Q_PAD_SD3_DAT6__USDHC3_DAT6_50MHZ, //IOMUX_PAD(0x0694, 0x02ac, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 0 000 694 2ac,
MX6Q_PAD_SD3_DAT7__USDHC3_DAT7_50MHZ, //IOMUX_PAD(0x0690, 0x02a8, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 0 000 690 2a8,
};

static iomux_v3_cfg_t mx6q_sd3_100mhz[] = {
MX6Q_PAD_SD3_CLK__USDHC3_CLK_100MHZ, //IOMUX_PAD(0x06a4, 0x02bc, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 0 000 6a4 2bc,
MX6Q_PAD_SD3_CMD__USDHC3_CMD_100MHZ, //IOMUX_PAD(0x06a0, 0x02b8, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e163 0 000 6a0 2b8,
MX6Q_PAD_SD3_DAT0__USDHC3_DAT0_100MHZ, //IOMUX_PAD(0x06a8, 0x02c0, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 0 000 6a8 2c0,
MX6Q_PAD_SD3_DAT1__USDHC3_DAT1_100MHZ, //IOMUX_PAD(0x06ac, 0x02c4, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 0 000 6ac 2c4,
MX6Q_PAD_SD3_DAT2__USDHC3_DAT2_100MHZ, //IOMUX_PAD(0x06b0, 0x02c8, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 0 000 6b0 2c8,
MX6Q_PAD_SD3_DAT3__USDHC3_DAT3_100MHZ, //IOMUX_PAD(0x06b4, 0x02cc, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 0 000 6b4 2cc,
MX6Q_PAD_SD3_DAT4__USDHC3_DAT4_100MHZ, //IOMUX_PAD(0x069c, 0x02b4, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 0 000 69c 2b4,
MX6Q_PAD_SD3_DAT5__USDHC3_DAT5_100MHZ, //IOMUX_PAD(0x0698, 0x02b0, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 0 000 698 2b0,
MX6Q_PAD_SD3_DAT6__USDHC3_DAT6_100MHZ, //IOMUX_PAD(0x0694, 0x02ac, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 0 000 694 2ac,
MX6Q_PAD_SD3_DAT7__USDHC3_DAT7_100MHZ, //IOMUX_PAD(0x0690, 0x02a8, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 0 000 690 2a8,
};

static iomux_v3_cfg_t mx6q_sd3_200mhz[] = {
MX6Q_PAD_SD3_CLK__USDHC3_CLK_200MHZ, //IOMUX_PAD(0x06a4, 0x02bc, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 0 000 6a4 2bc,
MX6Q_PAD_SD3_CMD__USDHC3_CMD_200MHZ, //IOMUX_PAD(0x06a0, 0x02b8, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e3 0 000 6a0 2b8,
MX6Q_PAD_SD3_DAT0__USDHC3_DAT0_200MHZ, //IOMUX_PAD(0x06a8, 0x02c0, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 0 000 6a8 2c0,
MX6Q_PAD_SD3_DAT1__USDHC3_DAT1_200MHZ, //IOMUX_PAD(0x06ac, 0x02c4, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 0 000 6ac 2c4,
MX6Q_PAD_SD3_DAT2__USDHC3_DAT2_200MHZ, //IOMUX_PAD(0x06b0, 0x02c8, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 0 000 6b0 2c8,
MX6Q_PAD_SD3_DAT3__USDHC3_DAT3_200MHZ, //IOMUX_PAD(0x06b4, 0x02cc, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 0 000 6b4 2cc,
MX6Q_PAD_SD3_DAT4__USDHC3_DAT4_200MHZ, //IOMUX_PAD(0x069c, 0x02b4, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 0 000 69c 2b4,
MX6Q_PAD_SD3_DAT5__USDHC3_DAT5_200MHZ, //IOMUX_PAD(0x0698, 0x02b0, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 0 000 698 2b0,
MX6Q_PAD_SD3_DAT6__USDHC3_DAT6_200MHZ, //IOMUX_PAD(0x0694, 0x02ac, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 0 000 694 2ac,
MX6Q_PAD_SD3_DAT7__USDHC3_DAT7_200MHZ, //IOMUX_PAD(0x0690, 0x02a8, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 0 000 690 2a8,
};

static iomux_v3_cfg_t mx6dl_sd3_50mhz[] = {
0x0be0e209346f430c,
0x03e0e300006f8310,
0x03e0e200006fc314,
0x03e0e20000700318,
0x03e0e2000070431c,
0x03e0e20000708320,
0x03e0e2000070c324,
0x03e0e20000710328,
0x03e0e2000071432c,
0x03e0e20000718330,
};

static iomux_v3_cfg_t mx6dl_sd3_100mhz[] = {
0x0ae16209346f430c,
0x02e16300006f8310,
0x02e16200006fc314,
0x02e1620000700318,
0x02e162000070431c,
0x02e1620000708320,
0x02e162000070c324,
0x02e1620000710328,
0x02e162000071432c,
0x02e1620000718330,
};

static iomux_v3_cfg_t mx6dl_sd3_200mhz[] = {
0x0ae1f209346f430c,
0x02e1f200006f8310,
0x02e1f200006fc314,
0x02e1f20000700318,
0x02e1f2000070431c,
0x02e1f20000708320,
0x02e1f2000070c324,
0x02e1f20000710328,
0x02e1f2000071432c,
0x02e1f20000718330,
};

static iomux_v3_cfg_t mx6q_sd4_50mhz[] = {
MX6Q_PAD_SD4_CLK__USDHC4_CLK_50MHZ, //IOMUX_PAD(0x06e0, 0x02f8, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 0 000 6e0 2f8,
MX6Q_PAD_SD4_CMD__USDHC4_CMD_50MHZ, //IOMUX_PAD(0x06dc, 0x02f4, 0, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b3 0 000 6dc 2f4,
MX6Q_PAD_SD4_DAT0__USDHC4_DAT0_50MHZ, //IOMUX_PAD(0x0704, 0x031c, 1, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 1 000 704 31c,
MX6Q_PAD_SD4_DAT1__USDHC4_DAT1_50MHZ, //IOMUX_PAD(0x0708, 0x0320, 1, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 1 000 708 320,
MX6Q_PAD_SD4_DAT2__USDHC4_DAT2_50MHZ, //IOMUX_PAD(0x070c, 0x0324, 1, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 1 000 70c 324,
MX6Q_PAD_SD4_DAT3__USDHC4_DAT3_50MHZ, //IOMUX_PAD(0x0710, 0x0328, 1, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 1 000 710 328,
MX6Q_PAD_SD4_DAT4__USDHC4_DAT4_50MHZ, //IOMUX_PAD(0x0714, 0x032c, 1, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 1 000 714 32c,
MX6Q_PAD_SD4_DAT5__USDHC4_DAT5_50MHZ, //IOMUX_PAD(0x0718, 0x0330, 1, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 1 000 718 330,
MX6Q_PAD_SD4_DAT6__USDHC4_DAT6_50MHZ, //IOMUX_PAD(0x071c, 0x0334, 1, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 1 000 71c 334,
MX6Q_PAD_SD4_DAT7__USDHC4_DAT7_50MHZ, //IOMUX_PAD(0x0720, 0x0338, 1, 0x0000, 0, 0x17059) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e0b2 1 000 720 338,
};

static iomux_v3_cfg_t mx6q_sd4_100mhz[] = {
MX6Q_PAD_SD4_CLK__USDHC4_CLK_100MHZ, //IOMUX_PAD(0x06e0, 0x02f8, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 0 000 6e0 2f8,
MX6Q_PAD_SD4_CMD__USDHC4_CMD_100MHZ, //IOMUX_PAD(0x06dc, 0x02f4, 0, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e163 0 000 6dc 2f4,
MX6Q_PAD_SD4_DAT0__USDHC4_DAT0_100MHZ, //IOMUX_PAD(0x0704, 0x031c, 1, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 1 000 704 31c,
MX6Q_PAD_SD4_DAT1__USDHC4_DAT1_100MHZ, //IOMUX_PAD(0x0708, 0x0320, 1, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 1 000 708 320,
MX6Q_PAD_SD4_DAT2__USDHC4_DAT2_100MHZ, //IOMUX_PAD(0x070c, 0x0324, 1, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 1 000 70c 324,
MX6Q_PAD_SD4_DAT3__USDHC4_DAT3_100MHZ, //IOMUX_PAD(0x0710, 0x0328, 1, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 1 000 710 328,
MX6Q_PAD_SD4_DAT4__USDHC4_DAT4_100MHZ, //IOMUX_PAD(0x0714, 0x032c, 1, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 1 000 714 32c,
MX6Q_PAD_SD4_DAT5__USDHC4_DAT5_100MHZ, //IOMUX_PAD(0x0718, 0x0330, 1, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 1 000 718 330,
MX6Q_PAD_SD4_DAT6__USDHC4_DAT6_100MHZ, //IOMUX_PAD(0x071c, 0x0334, 1, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 1 000 71c 334,
MX6Q_PAD_SD4_DAT7__USDHC4_DAT7_100MHZ, //IOMUX_PAD(0x0720, 0x0338, 1, 0x0000, 0, 0x170B1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e162 1 000 720 338,
};

static iomux_v3_cfg_t mx6q_sd4_200mhz[] = {
MX6Q_PAD_SD4_CLK__USDHC4_CLK_200MHZ, //IOMUX_PAD(0x06e0, 0x02f8, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 0 000 6e0 2f8,
MX6Q_PAD_SD4_CMD__USDHC4_CMD_200MHZ, //IOMUX_PAD(0x06dc, 0x02f4, 0, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e3 0 000 6dc 2f4,
MX6Q_PAD_SD4_DAT0__USDHC4_DAT0_200MHZ, //IOMUX_PAD(0x0704, 0x031c, 1, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 1 000 704 31c,
MX6Q_PAD_SD4_DAT1__USDHC4_DAT1_200MHZ, //IOMUX_PAD(0x0708, 0x0320, 1, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 1 000 708 320,
MX6Q_PAD_SD4_DAT2__USDHC4_DAT2_200MHZ, //IOMUX_PAD(0x070c, 0x0324, 1, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 1 000 70c 324,
MX6Q_PAD_SD4_DAT3__USDHC4_DAT3_200MHZ, //IOMUX_PAD(0x0710, 0x0328, 1, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 1 000 710 328,
MX6Q_PAD_SD4_DAT4__USDHC4_DAT4_200MHZ, //IOMUX_PAD(0x0714, 0x032c, 1, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 1 000 714 32c,
MX6Q_PAD_SD4_DAT5__USDHC4_DAT5_200MHZ, //IOMUX_PAD(0x0718, 0x0330, 1, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 1 000 718 330,
MX6Q_PAD_SD4_DAT6__USDHC4_DAT6_200MHZ, //IOMUX_PAD(0x071c, 0x0334, 1, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 1 000 71c 334,
MX6Q_PAD_SD4_DAT7__USDHC4_DAT7_200MHZ, //IOMUX_PAD(0x0720, 0x0338, 1, 0x0000, 0, 0x170F1) /*| MUX_PAD_CTRL(NO_PAD_CTRL)*/, //0x02e1e2 1 000 720 338,
};

static iomux_v3_cfg_t mx6dl_sd4_50mhz[] = {
0x0be0e20938720338,
0x03e0e3000072433c,
0x03e0e21000728340,
0x03e0e2100072c344,
0x03e0e21000730348,
0x03e0e2100073434c,
0x03e0e21000738350,
0x03e0e2100073c354,
0x03e0e21000740358,
0x03e0e2100074435c,
};

static iomux_v3_cfg_t mx6dl_sd4_100mhz[] = {
0x0ae1620938720338,
0x02e163000072433c,
0x02e1621000728340,
0x02e162100072c344,
0x02e1621000730348,
0x02e162100073434c,
0x02e1621000738350,
0x02e162100073c354,
0x02e1621000740358,
0x02e162100074435c,
};

static iomux_v3_cfg_t mx6dl_sd4_200mhz[] = {
0x0ae1f20938720338,
0x02e1f3000072433c,
0x02e1f21000728340,
0x02e1f2100072c344,
0x02e1f21000730348,
0x02e1f2100073434c,
0x02e1f21000738350,
0x02e1f2100073c354,
0x02e1f21000740358,
0x02e1f2100074435c,
};

enum sd_pad_mode {
	SD_PAD_MODE_LOW_SPEED,
	SD_PAD_MODE_MED_SPEED,
	SD_PAD_MODE_HIGH_SPEED,
};

static int plt_sd3_pad_change(/*unsigned int index,*/ int clock)
{
	/* LOW speed is the default state of SD pads */
	static enum sd_pad_mode pad_mode = SD_PAD_MODE_MED_SPEED; //C07C23A0

	iomux_v3_cfg_t *sd_pads_200mhz = NULL; //ip
	iomux_v3_cfg_t *sd_pads_100mhz = NULL; //r4
	iomux_v3_cfg_t *sd_pads_50mhz = NULL; //r1

	u32 sd_pads_200mhz_cnt;
	u32 sd_pads_100mhz_cnt;
	u32 sd_pads_50mhz_cnt;

	if (cpu_is_mx6q()) {
		//c0061c74
		sd_pads_200mhz = mx6q_sd3_200mhz;
		sd_pads_100mhz = mx6q_sd3_100mhz;
		sd_pads_50mhz = mx6q_sd3_50mhz;

		sd_pads_200mhz_cnt = ARRAY_SIZE(mx6q_sd3_200mhz);
		sd_pads_100mhz_cnt = ARRAY_SIZE(mx6q_sd3_100mhz);
		sd_pads_50mhz_cnt = ARRAY_SIZE(mx6q_sd3_50mhz);
	} else if (cpu_is_mx6dl()) {
		sd_pads_200mhz = mx6dl_sd3_200mhz;
		sd_pads_100mhz = mx6dl_sd3_100mhz;
		sd_pads_50mhz = mx6dl_sd3_50mhz;

		sd_pads_200mhz_cnt = ARRAY_SIZE(mx6dl_sd3_200mhz);
		sd_pads_100mhz_cnt = ARRAY_SIZE(mx6dl_sd3_100mhz);
		sd_pads_50mhz_cnt = ARRAY_SIZE(mx6dl_sd3_50mhz);
	}

	if (clock > 100000000) {
		//c0061bcc
		if (pad_mode == SD_PAD_MODE_HIGH_SPEED)
			return 0;
		BUG_ON(!sd_pads_200mhz); //ip
		pad_mode = SD_PAD_MODE_HIGH_SPEED;
		return mxc_iomux_v3_setup_multiple_pads(sd_pads_200mhz,
							sd_pads_200mhz_cnt);
	} else if (clock > 52000000) {
		//c0061c0c
		if (pad_mode == SD_PAD_MODE_MED_SPEED)
			return 0;
		BUG_ON(!sd_pads_100mhz); //r4
		pad_mode = SD_PAD_MODE_MED_SPEED;
		return mxc_iomux_v3_setup_multiple_pads(sd_pads_100mhz,
							sd_pads_100mhz_cnt);
	} else {
		//c0061c3c
		if (pad_mode == SD_PAD_MODE_LOW_SPEED)
			return 0;
		BUG_ON(!sd_pads_50mhz); //r1
		pad_mode = SD_PAD_MODE_LOW_SPEED;
		return mxc_iomux_v3_setup_multiple_pads(sd_pads_50mhz,
							sd_pads_50mhz_cnt);
	}

	return 0;
}

static int plt_sd4_pad_change(/*unsigned int index,*/ int clock)
{
	/* LOW speed is the default state of SD pads */
	static enum sd_pad_mode pad_mode = SD_PAD_MODE_MED_SPEED; //C07C2588

	iomux_v3_cfg_t *sd_pads_200mhz = NULL; //ip
	iomux_v3_cfg_t *sd_pads_100mhz = NULL; //r4
	iomux_v3_cfg_t *sd_pads_50mhz = NULL; //r1

	u32 sd_pads_200mhz_cnt;
	u32 sd_pads_100mhz_cnt;
	u32 sd_pads_50mhz_cnt;

	if (cpu_is_mx6q()) {
		sd_pads_200mhz = mx6q_sd4_200mhz;
		sd_pads_100mhz = mx6q_sd4_100mhz;
		sd_pads_50mhz = mx6q_sd4_50mhz;

		sd_pads_200mhz_cnt = ARRAY_SIZE(mx6q_sd4_200mhz);
		sd_pads_100mhz_cnt = ARRAY_SIZE(mx6q_sd4_100mhz);
		sd_pads_50mhz_cnt = ARRAY_SIZE(mx6q_sd4_50mhz);
	} else if (cpu_is_mx6dl()) {
		sd_pads_200mhz = mx6dl_sd4_200mhz;
		sd_pads_100mhz = mx6dl_sd4_100mhz;
		sd_pads_50mhz = mx6dl_sd4_50mhz;

		sd_pads_200mhz_cnt = ARRAY_SIZE(mx6dl_sd4_200mhz);
		sd_pads_100mhz_cnt = ARRAY_SIZE(mx6dl_sd4_100mhz);
		sd_pads_50mhz_cnt = ARRAY_SIZE(mx6dl_sd4_50mhz);
	}

	if (clock > 100000000) {
		if (pad_mode == SD_PAD_MODE_HIGH_SPEED)
			return 0;
//		BUG_ON(!sd_pads_200mhz); //ip
		pad_mode = SD_PAD_MODE_HIGH_SPEED;
		return mxc_iomux_v3_setup_multiple_pads(sd_pads_200mhz,
							sd_pads_200mhz_cnt);
	} else if (clock > 52000000) {
		if (pad_mode == SD_PAD_MODE_MED_SPEED)
			return 0;
//		BUG_ON(!sd_pads_100mhz); //r4
		pad_mode = SD_PAD_MODE_MED_SPEED;
		return mxc_iomux_v3_setup_multiple_pads(sd_pads_100mhz,
							sd_pads_100mhz_cnt);
	} else {
		if (pad_mode == SD_PAD_MODE_LOW_SPEED)
			return 0;
//		BUG_ON(!sd_pads_50mhz); //r1
		pad_mode = SD_PAD_MODE_LOW_SPEED;
		return mxc_iomux_v3_setup_multiple_pads(sd_pads_50mhz,
							sd_pads_50mhz_cnt);
	}

	return 0;
}

static const struct esdhc_platform_data mx6q_richtechie_sd3_data __initconst = {
	.cd_gpio = RICHTECHIE_SD3_CD,
	.keep_power_at_suspend = 1,
	.support_8bit = 0, //1,
	.delay_line = 5, //0,
	.cd_type = ESDHC_CD_CONTROLLER,
	.runtime_pm = true,
	.platform_pad_change = plt_sd3_pad_change,
};

static const struct esdhc_platform_data mx6q_richtechie_sd4_data __initconst = {
	.always_present = 1,
	.keep_power_at_suspend = 1,
	.support_8bit = 1,
	.delay_line = 5, //0,
	.cd_type = ESDHC_CD_PERMANENT,
	.platform_pad_change = plt_sd4_pad_change,
};


static const struct imxuart_platform_data mx6_richtechie_uart4_data __initconst = {
	.flags      = IMXUART_HAVE_RTSCTS/* | IMXUART_USE_DCEDTE | IMXUART_SDMA*/,
	.dma_req_rx = MX6Q_DMA_REQ_UART5_RX,
	.dma_req_tx = MX6Q_DMA_REQ_UART5_TX,
};

static const struct anatop_thermal_platform_data
	mx6q_richtechie_anatop_thermal_data __initconst = {
		.name = "anatop_thermal",
};

static inline void mx6q_richtechie_init_uart(void)
{
	imx6q_add_imx_uart(3, NULL);
	imx6q_add_imx_uart(4, &mx6_richtechie_uart4_data);
}

static struct imx_ssi_platform_data mx6_richtechie_ssi_pdata = {
	.flags = IMX_SSI_DMA | IMX_SSI_SYN,
};

static struct platform_device mx6_richtechie_audio_wm8960_device = {
	.name = "imx-wm8960",
};

static struct mxc_audio_platform_data wm8960_data = {
	.ssi_num = 1,
	.src_port = 2,
	.ext_port = 4,
	.hp_gpio = RICHTECHIE_HEADPHONE_DET,
	.hp_active_low = 1,
	.mic_active_low = 1,
};

static int mxc_wm8960_init(void)
{
	struct clk *clko; //r4
	struct clk *new_parent; //r5
	int rate;

	clko = clk_get(NULL, "clko_clk");
	if (IS_ERR(clko)) {
		pr_err("can't get CLKO clock.\n");
		return PTR_ERR(clko);
	}

	new_parent = clk_get(NULL, "ipg_clk");
	if (!IS_ERR(new_parent)) {
		clk_set_parent(clko, new_parent);
		clk_put(new_parent);
	}

	printk("clko clk = %lu \n", clk_get_rate(clko));
	printk("clko parent clk = %lu \n", clk_get_rate(new_parent));

	/* both audio codec and comera use CLKO clk*/
	rate = clk_round_rate(clko, 12000000);

	wm8960_data.sysclk = rate;
	clk_set_rate(clko, rate);
	clk_enable(clko);

	return 0;
}


/*static*/ void richtechie_init_lcd(void)
{
	gpio_request(RICHTECHIE_LDB_BACKLIGHT, "ldb-backlight");
	gpio_direction_output(RICHTECHIE_LDB_BACKLIGHT, 0);

	gpio_request(RICHTECHIE_LDB_PWR, "ldb-pwr");
	gpio_direction_output(RICHTECHIE_LDB_PWR, 0);

	gpio_set_value(RICHTECHIE_LDB_PWR, 0);
	gpio_set_value(RICHTECHIE_LDB_PWR, 0);

	msleep(200);

	gpio_set_value(RICHTECHIE_LDB_BACKLIGHT, 1);

	gpio_request(RICHTECHIE_LCD_EN, "lcd_en");

	msleep(10);

	gpio_direction_output(RICHTECHIE_LCD_EN, 1);
	gpio_set_value(RICHTECHIE_LCD_EN, 1);
}


static void camera_pwdn_control/*mx6q_csi0_cam_powerdown*/(int powerdown)
{
#if 0
	if (powerdown)
		gpio_set_value(SABRESD_CSI0_PWN, 1);
	else
		gpio_set_value(SABRESD_CSI0_PWN, 0);

	msleep(2);
#endif
}

static void mx6q_csi0_io_init(void)
{
#if 0
	if (cpu_is_mx6q())
		mxc_iomux_v3_setup_multiple_pads(mx6q_sabresd_csi0_sensor_pads,
			ARRAY_SIZE(mx6q_sabresd_csi0_sensor_pads));
	else if (cpu_is_mx6dl())
		mxc_iomux_v3_setup_multiple_pads(mx6dl_sabresd_csi0_sensor_pads,
			ARRAY_SIZE(mx6dl_sabresd_csi0_sensor_pads));

	/* Camera reset */
	gpio_request(SABRESD_CSI0_RST, "cam-reset");
	gpio_direction_output(SABRESD_CSI0_RST, 1);

	/* Camera power down */
	gpio_request(SABRESD_CSI0_PWN, "cam-pwdn");
	gpio_direction_output(SABRESD_CSI0_PWN, 1);
	msleep(5);
	gpio_set_value(SABRESD_CSI0_PWN, 0);
	msleep(5);
	gpio_set_value(SABRESD_CSI0_RST, 0);
	msleep(1);
	gpio_set_value(SABRESD_CSI0_RST, 1);
	msleep(5);
	gpio_set_value(SABRESD_CSI0_PWN, 1);

	/* For MX6Q:
	 * GPR1 bit19 and bit20 meaning:
	 * Bit19:       0 - Enable mipi to IPU1 CSI0
	 *                      virtual channel is fixed to 0
	 *              1 - Enable parallel interface to IPU1 CSI0
	 * Bit20:       0 - Enable mipi to IPU2 CSI1
	 *                      virtual channel is fixed to 3
	 *              1 - Enable parallel interface to IPU2 CSI1
	 * IPU1 CSI1 directly connect to mipi csi2,
	 *      virtual channel is fixed to 1
	 * IPU2 CSI0 directly connect to mipi csi2,
	 *      virtual channel is fixed to 2
	 *
	 * For MX6DL:
	 * GPR13 bit 0-2 IPU_CSI0_MUX
	 *   000 MIPI_CSI0
	 *   100 IPU CSI0
	 */
	if (cpu_is_mx6q())
		mxc_iomux_set_gpr_register(1, 19, 1, 1);
	else if (cpu_is_mx6dl())
		mxc_iomux_set_gpr_register(13, 0, 3, 4);
#endif
}

struct
{
	int dummy;
} Data_c07c36e8 = {0xa9};


static void init_battery_adp_det(void)
{
	gpio_request(RICHTECHIE_AC_DET, "ac_det");
	gpio_direction_input(RICHTECHIE_AC_DET);

	gpio_request(RICHTECHIE_AC_CHARGING, "ac_charging");
	gpio_direction_input(RICHTECHIE_AC_CHARGING);
}

static int get_ac_status(void)
{
	return gpio_get_value(RICHTECHIE_AC_DET);
}

static struct rt_battery_platform_data Data_c07c371c = { //c07c371c
	.Func_12 = init_battery_adp_det,
	.Func_0 = get_ac_status,
	.irq = 294,
	/*todo*/
};

static struct fsl_mxc_camera_platform_data camera_data = {
	.mclk = 24000000,
	.mclk_source = 0,
	.csi = 0,
	.io_init = mx6q_csi0_io_init,
	.pwdn = camera_pwdn_control, //mx6q_csi0_cam_powerdown,
};


static struct imxi2c_platform_data mx6q_richtechie_i2c0_data = {
	.bitrate = 200000,
};

static struct imxi2c_platform_data mx6q_richtechie_i2c1_data = {
	.bitrate = 400000,
};

static struct imxi2c_platform_data mx6q_richtechie_i2c2_data = {
	.bitrate = 7000,
};

static struct i2c_board_info mxc_i2c0_board_info[] __initdata = {
	{
		I2C_BOARD_INFO("gc0308", 0x21),
		.platform_data = (void *)&camera_data,
	},
	{
		I2C_BOARD_INFO("hi704", 0x30),
		.platform_data = (void *)&camera_data,
	},
};

static struct i2c_board_info mxc_i2c1_board_info[] __initdata = {
	{
		I2C_BOARD_INFO("mxc_hdmi_i2c", 0x50),
	},
	{
		I2C_BOARD_INFO("ft5x0x_ts", 0x38),
		.platform_data = (void *)&Data_c07c36e8,
		.irq = gpio_to_irq(RICHTECHIE_TS_INT),
	},
	{
		I2C_BOARD_INFO("mma8452", 0x1c),
	},
	{
		I2C_BOARD_INFO("wm8960", 0x1a),
	},
};

static struct i2c_board_info mxc_i2c2_board_info[] __initdata = {
	{
		I2C_BOARD_INFO("rbsys-i2c", 0x50),
		.platform_data = (void *)0,
	},
	{
		I2C_BOARD_INFO("rt_battery-i2c", 0x10),
		.platform_data = &Data_c07c371c,
	},
};


static void imx6q_richtechie_usbotg_vbus(bool on)
{
	if (on)
		gpio_set_value(RICHTECHIE_USB_OTG_PWR, 1);
	else
		gpio_set_value(RICHTECHIE_USB_OTG_PWR, 0);
}

static void __init imx6q_richtechie_init_usb(void)
{
	int ret = 0;

	imx_otg_base = MX6_IO_ADDRESS(MX6Q_USB_OTG_BASE_ADDR);
	/* disable external charger detect,
	 * or it will affect signal quality at dp .
	 */
	ret = gpio_request(RICHTECHIE_USB_OTG_PWR, "usb-pwr");
	if (ret) {
		pr_err("failed to get GPIO MX6Q_RT_USB_OTG_PWR: %d\n",
			ret);
		return;
	}
	gpio_direction_output(RICHTECHIE_USB_OTG_PWR, 0);
#if 0
	/* keep USB host1 VBUS always on */
	ret = gpio_request(SABRESD_USB_H1_PWR, "usb-h1-pwr");
	if (ret) {
		pr_err("failed to get GPIO SABRESD_USB_H1_PWR: %d\n",
			ret);
		return;
	}
	gpio_direction_output(SABRESD_USB_H1_PWR, 1);
#endif
	mxc_iomux_set_gpr_register(1, 13, 1, 1);
	mx6_set_otghost_vbus_func(imx6q_richtechie_usbotg_vbus);
	mx6_usb_dr_init();
}

static struct viv_gpu_platform_data imx6q_gpu_pdata __initdata = {
	.reserved_mem_size = SZ_128M + SZ_64M,
};

static struct imx_asrc_platform_data imx_asrc_data = {
	.channel_bits = 4,
	.clk_map_ver = 2,
};


static struct ipuv3_fb_platform_data richtechie_fb_data[] = {
	{ /*fb0*/
	.disp_dev = "ldb",
	.interface_pix_fmt = IPU_PIX_FMT_RGB666,
	.mode_str = "LDB-XGA",
	.default_bpp = 16,
	.int_clk = false,
	}, {
	.disp_dev = "hdmi",
	.interface_pix_fmt = IPU_PIX_FMT_RGB24,
	.mode_str = "1920x1080M@60",
	.default_bpp = 32,
	.int_clk = false,
	},
};


static void hdmi_init(int ipu_id, int disp_id)
{
	int hdmi_mux_setting;

	if ((ipu_id > 1) || (ipu_id < 0)) {
		pr_err("Invalid IPU select for HDMI: %d. Set to 0\n", ipu_id);
		ipu_id = 0;
	}

	if ((disp_id > 1) || (disp_id < 0)) {
		pr_err("Invalid DI select for HDMI: %d. Set to 0\n", disp_id);
		disp_id = 0;
	}

	/* Configure the connection between IPU1/2 and HDMI */
	hdmi_mux_setting = 2*ipu_id + disp_id;

	/* GPR3, bits 2-3 = HDMI_MUX_CTL */
	mxc_iomux_set_gpr_register(3, 2, 2, hdmi_mux_setting);

	/* Set HDMI event as SDMA event2 while Chip version later than TO1.2 */
	if (hdmi_SDMA_check())
		mxc_iomux_set_gpr_register(0, 0, 1, 1);
}

static struct fsl_mxc_hdmi_platform_data hdmi_data = {
	.init = hdmi_init,
};

static struct fsl_mxc_hdmi_core_platform_data hdmi_core_data = {
	.ipu_id = 0,
	.disp_id = 0,
};


static struct fsl_mxc_ldb_platform_data ldb_data = {
	.ipu_id = 1,
	.disp_id = 1,
	.ext_ref = 1,
	.mode = LDB_SIN1, //LDB_SEP1,
	.sec_ipu_id = 0,
	.sec_disp_id = 0,
};


static struct imx_ipuv3_platform_data ipu_data[] = {
	{
	.rev = 4,
	.csi_clk[0] = "clko2_clk", //"clko_clk",
	}, {
	.rev = 4,
	.csi_clk[0] = "clko2_clk", //"clko_clk",
	},
};

#define GPIO_BUTTON(gpio_num, ev_code, act_low, descr, wake, debounce)	\
{								\
	.gpio		= gpio_num,				\
	.type		= EV_KEY,				\
	.code		= ev_code,				\
	.active_low	= act_low,				\
	.desc		= "btn " descr,				\
	.wakeup		= wake,					\
	.debounce_interval = debounce,				\
}

static struct gpio_keys_button richtechie_buttons[] = {
	GPIO_BUTTON(RICHTECHIE_POWER_KEY, KEY_POWER, 1, "key-power", 1, 0),
	GPIO_BUTTON(RICHTECHIE_BACK, KEY_BACK, 1, "key-back", 0, 0),
	GPIO_BUTTON(RICHTECHIE_VOLUME_UP, KEY_VOLUMEUP, 1, "volume-up", 0, 0),
	GPIO_BUTTON(RICHTECHIE_VOLUME_DN, KEY_VOLUMEDOWN, 1, "volume-down", 0, 0),
};

static struct gpio_keys_platform_data richtechie_button_data = {
	.buttons	= richtechie_buttons,
	.nbuttons	= ARRAY_SIZE(richtechie_buttons),
};

static struct platform_device richtechie_button_device = {
	.name		= "gpio-keys",
	.id		= -1,
	.num_resources  = 0,
	.dev		= {
		.platform_data = &richtechie_button_data,
	}
};

static void __init imx6q_add_device_buttons(void)
{
#if 0
	if (mx6q_revision() >= IMX_CHIP_REVISION_1_2 ||
			mx6dl_revision() >= IMX_CHIP_REVISION_1_1)
		platform_device_add_data(&richtechie_button_device,
				&new_sabresd_button_data,
				sizeof(new_sabresd_button_data));
	else
		platform_device_add_data(&richtechie_button_device,
				&sabresd_button_data,
				sizeof(sabresd_button_data));
#endif

	platform_device_register(&richtechie_button_device);
}

static struct platform_pwm_backlight_data mx6_richtechie_pwm_backlight_data = {
	.pwm_id		= 0,
	.max_brightness	= 255,
	.dft_brightness	= 100,
	.pwm_period_ns	= 500000,
};

static struct platform_device Data_C07C3220 = {
	.name = "rbsys",
};

static struct {int dummy; } Data_C07C3388 = {
		0
};

static struct platform_device Data_C07C33C0 = {
	.name = "rt_vibrator",
};

static struct {int dummy; } Data_C07C3528 = {
		0
};

static struct platform_device Data_C07C3538 = {
	.name = "rt_it6251",
};


static struct ion_platform_data imx_ion_data = {
	.nr = 1,
	.heaps = {
		{
		.type = ION_HEAP_TYPE_CARVEOUT,
		.name = "vpu_ion",
		.size = SZ_64M,
		},
	},
};


static struct fsl_mxc_capture_platform_data capture_data[] = {
	{
		.csi = 0,
		.ipu = 0,
		.mclk_source = 0,
		.is_mipi = 0,
	}, {
		.csi = 1,
		.ipu = 0,
		.mclk_source = 0,
		.is_mipi = 1,
	},
};

static void richtechie_suspend_enter(void)
{
	/* suspend preparation */
	/* Disable AUX 5V */
//	gpio_set_value(SABRESD_AUX_5V_EN, 0);
}

static void richtechie_suspend_exit(void)
{
	/* resume restore */
	/* Enable AUX 5V */
//	gpio_set_value(SABRESD_AUX_5V_EN, 1);
}
static const struct pm_platform_data mx6q_richtechie_pm_data __initconst = {
	.name = "imx_pm",
	.suspend_enter = richtechie_suspend_enter,
	.suspend_exit = richtechie_suspend_exit,
};

static struct regulator_consumer_supply richtechie_vmmc_consumers[] = {
	REGULATOR_SUPPLY("vmmc", "sdhci-esdhc-imx.2"),
	REGULATOR_SUPPLY("vmmc", "sdhci-esdhc-imx.3"),
};

static struct regulator_init_data richtechie_vmmc_init = {
	.num_consumer_supplies = ARRAY_SIZE(richtechie_vmmc_consumers),
	.consumer_supplies = richtechie_vmmc_consumers,
};

static struct fixed_voltage_config richtechie_vmmc_reg_config = {
	.supply_name		= "vmmc",
	.microvolts		= 3300000,
	.gpio			= -1,
	.init_data		= &richtechie_vmmc_init,
};

static struct platform_device richtechie_vmmc_reg_devices = {
	.name	= "reg-fixed-voltage",
	.id	= 0,
	.dev	= {
		.platform_data = &richtechie_vmmc_reg_config,
	},
};

static int __init imx6q_init_audio(void)
{
	gpio_request(RICHTECHIE_SPEAKER_EN, "speak_en");
	gpio_direction_output(RICHTECHIE_SPEAKER_EN, 0);
	gpio_set_value(RICHTECHIE_SPEAKER_EN, 0);

	mxc_register_device(&mx6_richtechie_audio_wm8960_device,
			    &wm8960_data);

	imx6q_add_imx_ssi(1, &mx6_richtechie_ssi_pdata);

	mxc_wm8960_init();

	return 0;
}


static struct mxc_dvfs_platform_data richtechie_dvfscore_data = {
#ifdef CONFIG_MX6_INTER_LDO_BYPASS
	.reg_id = "VDDCORE",
	.soc_id	= "VDDSOC",
#else
	.reg_id = "cpu_vddgp",
	.soc_id = "cpu_vddsoc",
	.pu_id = "cpu_vddvpu",
#endif
	.clk1_id = "cpu_clk",
	.clk2_id = "gpc_dvfs_clk",
	.gpc_cntr_offset = MXC_GPC_CNTR_OFFSET,
	.ccm_cdcr_offset = MXC_CCM_CDCR_OFFSET,
	.ccm_cacrr_offset = MXC_CCM_CACRR_OFFSET,
	.ccm_cdhipr_offset = MXC_CCM_CDHIPR_OFFSET,
	.prediv_mask = 0x1F800,
	.prediv_offset = 11,
	.prediv_val = 3,
	.div3ck_mask = 0xE0000000,
	.div3ck_offset = 29,
	.div3ck_val = 2,
	.emac_val = 0x08,
	.upthr_val = 25,
	.dnthr_val = 9,
	.pncthr_val = 33,
	.upcnt_val = 10,
	.dncnt_val = 10,
	.delay_time = 80,
};

static void __init fixup_mxc_board(struct machine_desc *desc, struct tag *tags,
				   char **cmdline, struct meminfo *mi)
{
	char *str;
	struct tag *t;
	int i = 0;
	struct ipuv3_fb_platform_data *pdata_fb = richtechie_fb_data;

	for_each_tag(t, tags) {
		if (t->hdr.tag == ATAG_CMDLINE) {
			str = t->u.cmdline.cmdline;
			str = strstr(str, "fbmem=");
			if (str != NULL) {
				str += 6;
				pdata_fb[i++].res_size[0] = memparse(str, &str);
				while (*str == ',' &&
					i < ARRAY_SIZE(richtechie_fb_data)) {
					str++;
					pdata_fb[i++].res_size[0] = memparse(str, &str);
				}
			}
			break;
		}
	}
}

#define SNVS_LPCR 0x38
static void rt_power_off(void)
{
	extern void mcu_power_off(void);

	void __iomem *mx6_snvs_base =  MX6_IO_ADDRESS(MX6Q_SNVS_BASE_ADDR);
	u32 value;
	value = readl(mx6_snvs_base + SNVS_LPCR);
	/*set TOP and DP_EN bit*/
	writel(value | 0x60, mx6_snvs_base + SNVS_LPCR);

	printk(" rt power off \015\n");

	mcu_power_off();

	gpio_request(RICHTECHIE_POWER_OFF, "Power_off");
	gpio_direction_output(RICHTECHIE_POWER_OFF, 0);
	gpio_set_value(RICHTECHIE_POWER_OFF, 0);
}


/*!
 * Board specific initialization.
 */
static void __init mx6_richtechie_board_init(void)
{
	int i;

	printk("---richtechie board init---\015\n");

	if (cpu_is_mx6q())
	{
		mxc_iomux_v3_setup_multiple_pads(mx6q_richtechie_pads1,
			ARRAY_SIZE(mx6q_richtechie_pads1));

		gpio_request(RICHTECHIE_SENSOR_INT1, "sensor_int1");
		gpio_request(RICHTECHIE_SENSOR_INT2, "sensor_int2");

		gpio_direction_input(RICHTECHIE_SENSOR_INT1);
		gpio_direction_input(RICHTECHIE_SENSOR_INT2);

		Data_c07fd670 = gpio_get_value(RICHTECHIE_SENSOR_INT2);

		printk("INT1 = %d, INT2 = %d \n",
			gpio_get_value(RICHTECHIE_SENSOR_INT1),
			gpio_get_value(RICHTECHIE_SENSOR_INT2));

		mxc_iomux_v3_setup_multiple_pads(mx6q_richtechie_pads2,
			ARRAY_SIZE(mx6q_richtechie_pads2));
	}
	else if (cpu_is_mx6dl())
	{
		mxc_iomux_v3_setup_multiple_pads(mx6dl_richtechie_pads1,
			ARRAY_SIZE(mx6dl_richtechie_pads1));

		gpio_request(RICHTECHIE_SENSOR_INT1, "sensor_int1");
		gpio_request(RICHTECHIE_SENSOR_INT2, "sensor_int2");

		gpio_direction_input(RICHTECHIE_SENSOR_INT1);
		gpio_direction_input(RICHTECHIE_SENSOR_INT2);

		Data_c07fd670 = gpio_get_value(RICHTECHIE_SENSOR_INT2);

		mxc_iomux_v3_setup_multiple_pads(mx6dl_richtechie_pads2,
			ARRAY_SIZE(mx6dl_richtechie_pads2));
	}

	gp_reg_id = richtechie_dvfscore_data.reg_id;
	soc_reg_id = richtechie_dvfscore_data.soc_id;
	pu_reg_id = richtechie_dvfscore_data.pu_id;
	mx6q_richtechie_init_uart();

	/*
	 * MX6DL/Solo only supports single IPU
	 * The following codes are used to change ipu id
	 * and display id information for MX6DL/Solo. Then
	 * register 1 IPU device and up to 2 displays for
	 * MX6DL/Solo
	 */
	if (cpu_is_mx6dl()) {
		ldb_data.ipu_id = 0;
		ldb_data.disp_id = 1;
		hdmi_core_data.disp_id = 0;
//		mipi_dsi_pdata.ipu_id = 0;
//		mipi_dsi_pdata.disp_id = 1;
	}
	imx6q_add_mxc_hdmi_core(&hdmi_core_data);

	imx6q_add_ipuv3(0, &ipu_data[0]);
	if (cpu_is_mx6q())
		imx6q_add_ipuv3(1, &ipu_data[1]);
	for (i = 0; i < ARRAY_SIZE(richtechie_fb_data); i++)
		imx6q_add_ipuv3fb(i, &richtechie_fb_data[i]);

	imx6q_add_vdoa();
	imx6q_add_ldb(&ldb_data);
	imx6q_add_v4l2_output(0);
	imx6q_add_v4l2_capture(0, &capture_data[0]);
	imx6q_add_imx_snvs_rtc();

	imx6q_add_imx_caam();

	imx6q_add_imx_i2c(0, &mx6q_richtechie_i2c0_data);
	imx6q_add_imx_i2c(1, &mx6q_richtechie_i2c1_data);
	imx6q_add_imx_i2c(2, &mx6q_richtechie_i2c2_data);
	i2c_register_board_info(0, mxc_i2c0_board_info,
			ARRAY_SIZE(mxc_i2c0_board_info));
	i2c_register_board_info(1, mxc_i2c1_board_info,
			ARRAY_SIZE(mxc_i2c1_board_info));
	i2c_register_board_info(2, mxc_i2c2_board_info,
			ARRAY_SIZE(mxc_i2c2_board_info));

	imx6q_add_mxc_hdmi(&hdmi_data);

	imx6q_add_anatop_thermal_imx(1, &mx6q_richtechie_anatop_thermal_data);

	imx6q_add_pm_imx(0, &mx6q_richtechie_pm_data);
	imx6q_add_sdhci_usdhc_imx(3, &mx6q_richtechie_sd4_data);
	imx6q_add_sdhci_usdhc_imx(2, &mx6q_richtechie_sd3_data);
	imx_add_viv_gpu(&imx6_gpu_data, &imx6q_gpu_pdata);
	imx6q_add_vpu();
	imx6q_richtechie_init_usb();
	imx6q_init_audio();
	platform_device_register(&richtechie_vmmc_reg_devices);
#ifndef CONFIG_MX6_INTER_LDO_BYPASS
	mx6_cpu_regulator_init();
#endif
	imx_asrc_data.asrc_core_clk = clk_get(NULL, "asrc_clk");
	imx_asrc_data.asrc_audio_clk = clk_get(NULL, "asrc_serial_clk");
	imx6q_add_asrc(&imx_asrc_data);
	imx6q_add_otp();
	imx6q_add_viim();
	imx6q_add_imx2_wdt(0, NULL);
	imx6q_add_dma();

	imx6q_add_dvfs_core(&richtechie_dvfscore_data);

	imx6q_add_ion(0, &imx_ion_data,
		sizeof(imx_ion_data) + sizeof(struct ion_platform_heap));

	imx6q_add_device_buttons();

	imx6q_add_mxc_pwm(0);
	imx6q_add_mxc_pwm_backlight(0, &mx6_richtechie_pwm_backlight_data);
	richtechie_init_lcd();

	imx6q_add_hdmi_soc();
	imx6q_add_hdmi_soc_dai();
	imx6q_add_perfmon(0);
	imx6q_add_perfmon(1);
	imx6q_add_perfmon(2);

	pm_power_off = rt_power_off;

	mxc_register_device(&Data_C07C3220,
			    &Data_C07C3388);
	mxc_register_device(&Data_C07C33C0,
			    &Data_C07C3528);
	mxc_register_device(&Data_C07C3538,
			    NULL);

	if (cpu_is_mx6dl()) {
		imx6dl_add_imx_pxp();
		imx6dl_add_imx_pxp_client();
	}

	imx6_add_armpmu();
	imx6q_add_busfreq();

	gpio_request(RICHTECHIE_USB_HOST_PWR_EN, "usb-HOST_PWR_EN");
	gpio_direction_output(RICHTECHIE_USB_HOST_PWR_EN, 1);
	gpio_set_value(RICHTECHIE_USB_HOST_PWR_EN, 1);

	gpio_request(RICHTECHIE_TEST_1, "TEST_1");
	gpio_direction_output(RICHTECHIE_TEST_1, 0);
	gpio_request(RICHTECHIE_TEST_2, "TEST_2");
	gpio_direction_output(RICHTECHIE_TEST_2, 0);
	gpio_request(RICHTECHIE_TEST_3, "TEST_3");
	gpio_direction_output(RICHTECHIE_TEST_3, 0);

	printk("---richtechie board init end---\015\n");
}

atomic_t Data_c07c36a0 = ATOMIC_INIT(1); //c07c36a0

extern void __iomem *twd_base;
static void __init mx6_richtechie_timer_init(void)
{
	struct clk *uart_clk;
#ifdef CONFIG_LOCAL_TIMERS
	twd_base = ioremap(LOCAL_TWD_ADDR, SZ_256);
/*2165*/	BUG_ON(!twd_base);
#endif
	mx6_clocks_init(32768, 24000000, 0, 0);

	uart_clk = clk_get_sys("imx-uart.0", NULL);
	early_console_setup(UART4_BASE_ADDR, uart_clk);
}

static struct sys_timer mx6_richtechie_timer = {
	.init   = mx6_richtechie_timer_init,
};

static void __init mx6q_richtechie_reserve(void)
{
	phys_addr_t phys;
	int i;

#if defined(CONFIG_MXC_GPU_VIV) || defined(CONFIG_MXC_GPU_VIV_MODULE)
	if (imx6q_gpu_pdata.reserved_mem_size) {
		phys = memblock_alloc_base(imx6q_gpu_pdata.reserved_mem_size,
					   SZ_4K, SZ_1G);
		memblock_remove(phys, imx6q_gpu_pdata.reserved_mem_size);
		imx6q_gpu_pdata.reserved_mem_base = phys;
	}
#endif

#if defined(CONFIG_ION)
	if (imx_ion_data.heaps[0].size) {
		phys = memblock_alloc(imx_ion_data.heaps[0].size, SZ_4K);
		memblock_remove(phys, imx_ion_data.heaps[0].size);
		imx_ion_data.heaps[0].base = phys;
	}
#endif

	for (i = 0; i < ARRAY_SIZE(richtechie_fb_data); i++)
		if (richtechie_fb_data[i].res_size[0]) {
			/* reserve for background buffer */
			phys = memblock_alloc(richtechie_fb_data[i].res_size[0],
						SZ_4K);
			memblock_remove(phys, richtechie_fb_data[i].res_size[0]);
			richtechie_fb_data[i].res_base[0] = phys;
		}
}

/*
 * initialize __mach_desc_MX6Q_HDMIDONGLE data structure.
 */
MACHINE_START(MX6Q_RICHTECHIE, "Freescale i.MX 6Quad Richtechie Board")
	/* Maintainer: Freescale Semiconductor, Inc. */
	.boot_params = MX6_PHYS_OFFSET + 0x100,
	.fixup = fixup_mxc_board,
	.map_io = mx6_map_io,
	.init_irq = mx6_init_irq,
	.init_machine = mx6_richtechie_board_init,
	.timer = &mx6_richtechie_timer,
	.reserve = mx6q_richtechie_reserve,
MACHINE_END

