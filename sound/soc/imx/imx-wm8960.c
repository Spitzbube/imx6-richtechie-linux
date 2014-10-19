/*
 * imx-wm8960.c
 *
 * Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/bitops.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/fsl_devices.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/jack.h>
#include <mach/dma.h>
#include <mach/clock.h>
#include <mach/audmux.h>
#include <mach/gpio.h>
#include <asm/mach-types.h>

#include "imx-ssi.h"
#include "../codecs/wm8960.h"

struct imx_priv {
	int sysclk;         /*mclk from the outside*/ //8
	int codec_sysclk;
	int dai_hifi;
	int hp_irq;
	int hp_status;
	int amic_irq;
	int amic_status;
	struct platform_device *pdev; //36
	struct clk *Data_40; //40
};
unsigned int sample_format = SNDRV_PCM_FMTBIT_S16_LE; //c07f31a8
static int wm8960_jack_func; //c0922154
static int wm8960_spk_func; //c0922154+4
static struct imx_priv card_priv; //c0922154 +8
static struct snd_soc_card snd_soc_card_imx;
//static struct snd_soc_codec *gcodec;

static const char *jack_function[] = { "Headphone", "Mic", "Line", "Headset", "Off"};

static const char *spk_function[] = { "On", "Off" };

static const struct soc_enum wm8960_enum[] = {
	SOC_ENUM_SINGLE_EXT(5, jack_function),
	SOC_ENUM_SINGLE_EXT(2, spk_function),
};

static int tcc_get_jack(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.enumerated.item[0] = wm8960_jack_func;
	return 0;
}

static int tcc_set_jack(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);

	if (wm8960_jack_func == ucontrol->value.enumerated.item[0])
		return 0;

	wm8960_jack_func = ucontrol->value.enumerated.item[0];
	if (wm8960_jack_func)
		snd_soc_dapm_enable_pin(&codec->dapm, "Headphone Jack");
	else
		snd_soc_dapm_disable_pin(&codec->dapm, "Headphone Jack");

	snd_soc_dapm_sync(&codec->dapm);
	return 1;
}

static int tcc_get_spk(struct snd_kcontrol *kcontrol,
			    struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.enumerated.item[0] = wm8960_spk_func;
	return 0;
}

static int tcc_set_spk(struct snd_kcontrol *kcontrol,
			    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);

	if (wm8960_spk_func == ucontrol->value.enumerated.item[0])
		return 0;

	wm8960_spk_func = ucontrol->value.enumerated.item[0];
	if (wm8960_spk_func)
		snd_soc_dapm_enable_pin(&codec->dapm, "Ext Spk");
	else
		snd_soc_dapm_disable_pin(&codec->dapm, "Ext Spk");

	snd_soc_dapm_sync(&codec->dapm);
	return 1;
}

static const struct snd_kcontrol_new imx_wm8960_controls[] = {
	SOC_ENUM_EXT("Jack Function", wm8960_enum[0], tcc_get_jack, tcc_set_jack),
	SOC_ENUM_EXT("Speaker Function", wm8960_enum[1], tcc_get_spk, tcc_set_spk),
};

static int imx_hifi_startup(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct imx_priv *priv = &card_priv;
	//struct mxc_audio_platform_data *plat = priv->pdev->dev.platform_data;

	if (!codec_dai->active)
//		plat->clock_enable(1);
		clk_enable(priv->Data_40);

	return 0;
}

static void imx_hifi_shutdown(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct imx_priv *priv = &card_priv;
	//struct mxc_audio_platform_data *plat = priv->pdev->dev.platform_data;

	if (!codec_dai->active)
//		plat->clock_enable(0);
		clk_disable(priv->Data_40);

	return;
}

struct _coeff_div
{
	unsigned int channels;
	unsigned int format;
	unsigned int rate;
	unsigned int Data_12;
	unsigned int Data_16;
	unsigned int Data_20;
	int fill_24; //24
	//28
};

static const struct _coeff_div wm8960_audio[10] = //c0607e74
{
	{1, 2, 8000,   6144000, 15, 3, 16},
	{2, 2, 8000,  12288000, 15, 6, 32},
	{2, 2, 16000, 12288000, 12, 3, 32},
	{2, 2, 32000, 12288000,  9, 1, 32},
	{2, 2, 48000, 12288000,  7, 0, 32},
	{2, 2, 11025, 11289600, 13, 4, 32},
	{2, 2, 22050, 11289600, 10, 2, 32},
	{2, 2, 44100, 11289600,  7, 0, 32},
	{2, 6, 48000, 12288000,  4, 0, 64},
	{2, 6, 44100, 11289600,  4, 0, 64},
};

static inline int get_coeff(unsigned int rate, unsigned int format, unsigned int channels)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(wm8960_audio); i++)
	{
		if ((wm8960_audio[i].rate == rate) &&
				(wm8960_audio[i].format == format) &&
				(wm8960_audio[i].channels == channels))
		{
			return i;
		}
	}
	return -EINVAL;
}

/* todo */
static int imx_hifi_hw_params(struct snd_pcm_substream *substream,
				     struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct imx_priv *priv = &card_priv;
	unsigned int format = params_format(params);
	unsigned int rate = params_rate(params);
	unsigned int channels = params_channels(params);
//	unsigned int sample_rate = 44100;
	int ret = 0;
	int i = 0;
	u32 dai_format;

	ret = get_coeff(rate, format, channels);
	if (ret < 0)
	{
		printk("Not find rate!(%s) \n", "imx_hifi_hw_params");
		return ret;
	}

	i = ret;

	dai_format = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
		SND_SOC_DAIFMT_CBM_CFM;

	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, dai_format);
	if (ret < 0)
	{
		printk("snd_soc_dai_set_fmt error!  ret = %d \n", ret);
		return ret;
	}

	/* set i.MX active slot mask */
	snd_soc_dai_set_tdm_slot(cpu_dai,
				 channels == 1 ? 0xfffffffe : 0xfffffffc,
				 channels == 1 ? 0xfffffffe : 0xfffffffc,
				 2, 32);

	dai_format = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
		SND_SOC_DAIFMT_CBM_CFM;

	/* set cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, dai_format);
	if (ret < 0)
	{
		printk("Cpu dai:: snd_soc_dai_set_fmt error!  ret = %d \n", ret);
		return ret;
	}

//	sample_rate = params_rate(params);
	sample_format = params_format(params);

	ret = snd_soc_dai_set_pll(codec_dai,
			1 /*WM8962_FLL_OSC*/, 1 /*WM8962_FLL_OSC*/,
			priv->sysclk, wm8960_audio[i].Data_12);
	if (ret < 0)
		pr_err("Failed to start FLL: %d\n", ret);

	snd_soc_dai_set_clkdiv(codec_dai, WM8960_SYSCLKSEL, wm8960_audio[i].Data_16);
	snd_soc_dai_set_clkdiv(codec_dai, WM8960_DACDIV, wm8960_audio[i].Data_20);
	snd_soc_dai_set_clkdiv(codec_dai, WM8960_SYSCLKDIV,
			WM8960_SYSCLK_MCLK | WM8960_SYSCLK_DIV_2);

	return 0;
}

static const struct snd_kcontrol_new controls[] = {
	SOC_DAPM_PIN_SWITCH("Ext Spk"),
};

/* imx card dapm widgets */
static const struct snd_soc_dapm_widget imx_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_MIC("Mic Jack", NULL),
	SND_SOC_DAPM_SPK("Ext Spk", NULL),
};

/* imx machine connections to the codec pins */
static const struct snd_soc_dapm_route audio_map[] = {
	{ "Headphone Jack", NULL, "HP_L" },
	{ "Headphone Jack", NULL, "HP_R" },

	{ "Ext Spk", NULL, "SPK_RP" },
	{ "Ext Spk", NULL, "SPK_RN" },
	{ "Ext Spk", NULL, "SPK_LP" },
	{ "Ext Spk", NULL, "SPK_LN" },

#if 0
	{ "MICBIAS", NULL, "AMIC" },
	{ "IN3R", NULL, "MICBIAS" },

	{ "DMIC", NULL, "MICBIAS" },
	{ "DMICDAT", NULL, "DMIC" },
#else
	{ "LINPUT1", "MICB", "Mic Jack" },
#endif
};

static void headphone_detect_handler(struct work_struct *wor)
{
	struct imx_priv *priv = &card_priv;
	struct platform_device *pdev = priv->pdev;
	struct mxc_audio_platform_data *plat = pdev->dev.platform_data;
	char *envp[3];
	char *buf;

	/*sysfs_notify(&pdev->dev.kobj, NULL, "headphone");*/
	priv->hp_status = gpio_get_value(plat->hp_gpio);

	/* setup a message for userspace headphone in */
	buf = kmalloc(32, GFP_ATOMIC);
	if (!buf) {
		pr_err("%s kmalloc failed\n", __func__);
		return;
	}

	if (priv->hp_status != plat->hp_active_low)
		snprintf(buf, 32, "STATE=%d", 1);
	else
		snprintf(buf, 32, "STATE=%d", 0);

	envp[0] = "NAME=headphone";
	envp[1] = buf;
	envp[2] = NULL;
	kobject_uevent_env(&pdev->dev.kobj, KOBJ_CHANGE, envp);

	printk("Headphone stat: %s \n", buf);

	kfree(buf);

	enable_irq(priv->hp_irq);

	return;
}

static DECLARE_DELAYED_WORK(hp_event, headphone_detect_handler);

static irqreturn_t imx_headphone_detect_handler(int irq, void *data)
{
	disable_irq_nosync(irq);
	schedule_delayed_work(&hp_event, msecs_to_jiffies(200));
	return IRQ_HANDLED;
}

static ssize_t show_headphone(struct device_driver *dev, char *buf)
{
	struct imx_priv *priv = &card_priv;
	struct platform_device *pdev = priv->pdev;
	struct mxc_audio_platform_data *plat = pdev->dev.platform_data;

	/* determine whether hp is plugged in */
	priv->hp_status = gpio_get_value(plat->hp_gpio);

	if (priv->hp_status != plat->hp_active_low)
		strcpy(buf, "headphone\n");
	else
		strcpy(buf, "speaker\n");

	return strlen(buf);
}

static DRIVER_ATTR(headphone, S_IRUGO | S_IWUSR, show_headphone, NULL);

#if 0
static void amic_detect_handler(struct work_struct *work)
{
	struct imx_priv *priv = &card_priv;
	struct platform_device *pdev = priv->pdev;
	struct mxc_audio_platform_data *plat = pdev->dev.platform_data;
	char *envp[3];
	char *buf;

	/* sysfs_notify(&pdev->dev.kobj, NULL, "amic"); */
	priv->amic_status = gpio_get_value(plat->mic_gpio);

	/* if amic is inserted, disable dmic */
	if (priv->amic_status != plat->mic_active_low)
		snd_soc_dapm_nc_pin(&gcodec->dapm, "DMIC");
	else
		snd_soc_dapm_enable_pin(&gcodec->dapm, "DMIC");

	/* setup a message for userspace headphone in */
	buf = kmalloc(32, GFP_ATOMIC);
	if (!buf) {
		pr_err("%s kmalloc failed\n", __func__);
		return;
	}

	if (priv->amic_status == 0)
		snprintf(buf, 32, "STATE=%d", 2);
	else
		snprintf(buf, 32, "STATE=%d", 0);

	envp[0] = "NAME=amic";
	envp[1] = buf;
	envp[2] = NULL;
	kobject_uevent_env(&pdev->dev.kobj, KOBJ_CHANGE, envp);
	kfree(buf);

	enable_irq(priv->amic_irq);
}

static DECLARE_DELAYED_WORK(amic_event, amic_detect_handler);

static irqreturn_t imx_amic_detect_handler(int irq, void *data)
{
	disable_irq_nosync(irq);
	schedule_delayed_work(&amic_event, msecs_to_jiffies(200));
	return IRQ_HANDLED;
}

static ssize_t show_amic(struct device_driver *dev, char *buf)
{
	struct imx_priv *priv = &card_priv;
	struct platform_device *pdev = priv->pdev;
	struct mxc_audio_platform_data *plat = pdev->dev.platform_data;

	/* determine whether amic is plugged in */
	priv->amic_status = gpio_get_value(plat->hp_gpio);

	if (priv->amic_status != plat->mic_active_low)
		strcpy(buf, "amic\n");
	else
		strcpy(buf, "dmic\n");

	return strlen(buf);
}

static DRIVER_ATTR(amic, S_IRUGO | S_IWUSR, show_amic, NULL);
#endif

static int imx_wm8960_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct imx_priv *priv = &card_priv;
	struct platform_device *pdev = priv->pdev;
	struct mxc_audio_platform_data *plat = pdev->dev.platform_data;
	int ret = 0;

	//gcodec = rtd->codec;

	snd_soc_add_controls(codec, imx_wm8960_controls,
				     ARRAY_SIZE(imx_wm8960_controls));

	/* Add imx specific widgets */
	snd_soc_dapm_new_controls(&codec->dapm, imx_dapm_widgets,
				  ARRAY_SIZE(imx_dapm_widgets));

	/* Set up imx specific audio path audio_map */
	snd_soc_dapm_add_routes(&codec->dapm, audio_map, ARRAY_SIZE(audio_map));

	snd_soc_dapm_enable_pin(&codec->dapm, "Headphone Jack");
	snd_soc_dapm_enable_pin(&codec->dapm, "Mic Jack");

	snd_soc_dapm_sync(&codec->dapm);

	if (plat->hp_gpio != -1) {
			gpio_request(plat->hp_gpio, "headphone-detect");

			gpio_direction_input(plat->hp_gpio);

			priv->hp_irq = gpio_to_irq(plat->hp_gpio);

			ret = request_irq(priv->hp_irq,
						imx_headphone_detect_handler,
						IRQ_TYPE_EDGE_BOTH, pdev->name, priv);

			if (ret < 0) {
				ret = -EINVAL;
				return ret;
			}

			ret = driver_create_file(pdev->dev.driver,
							&driver_attr_headphone);
			if (ret < 0) {
				ret = -EINVAL;
				return ret;
			}
		}

#if 0
	if (plat->mic_gpio != -1) {
		priv->amic_irq = gpio_to_irq(plat->mic_gpio);

		ret = request_irq(priv->amic_irq,
					imx_amic_detect_handler,
					IRQ_TYPE_EDGE_BOTH, pdev->name, priv);

		if (ret < 0) {
			ret = -EINVAL;
			return ret;
		}

		ret = driver_create_file(pdev->dev.driver, &driver_attr_amic);
		if (ret < 0) {
			ret = -EINVAL;
			return ret;
		}

		priv->amic_status = gpio_get_value(plat->mic_gpio);

		/* if amic is inserted, disable DMIC */
		if (priv->amic_status != plat->mic_active_low)
			snd_soc_dapm_nc_pin(&codec->dapm, "DMIC");
		else
			snd_soc_dapm_enable_pin(&codec->dapm, "DMIC");
	}
	else if (!snd_soc_dapm_get_pin_status(&codec->dapm, "DMICDAT"))
		snd_soc_dapm_nc_pin(&codec->dapm, "DMIC");
#endif

	return 0;
}

static struct snd_soc_ops imx_hifi_ops = {
	.startup = imx_hifi_startup,
	.shutdown = imx_hifi_shutdown,
	.hw_params = imx_hifi_hw_params,
};

static struct snd_soc_dai_link imx_dai[] = {
	{
		.name = "HiFi",
		.stream_name = "HiFi",
		.codec_dai_name	= "wm8960-hifi",
		.codec_name	= "wm8960-codec.1-001a",
		.cpu_dai_name	= "imx-ssi.1",
		.platform_name	= "imx-pcm-audio.1",
		.init		= imx_wm8960_init,
		.ops		= &imx_hifi_ops,
	},
};

static struct snd_soc_card snd_soc_card_imx = {
	.name		= "wm8960-audio",
	.dai_link	= imx_dai,
	.num_links	= ARRAY_SIZE(imx_dai),
};

static int imx_audmux_config(int slave, int master)
{
	unsigned int ptcr, pdcr;
	slave = slave - 1;
	master = master - 1;

	ptcr = MXC_AUDMUX_V2_PTCR_SYN |
		MXC_AUDMUX_V2_PTCR_TFSDIR |
		MXC_AUDMUX_V2_PTCR_TFSEL(master) |
		MXC_AUDMUX_V2_PTCR_TCLKDIR |
		MXC_AUDMUX_V2_PTCR_TCSEL(master);
	pdcr = MXC_AUDMUX_V2_PDCR_RXDSEL(master);
	mxc_audmux_v2_configure_port(slave, ptcr, pdcr);

	ptcr = MXC_AUDMUX_V2_PTCR_SYN;
	pdcr = MXC_AUDMUX_V2_PDCR_RXDSEL(slave);
	mxc_audmux_v2_configure_port(master, ptcr, pdcr);

	return 0;
}

/*
 * This function will register the snd_soc_pcm_link drivers.
 */
static int __devinit imx_wm8960_probe(struct platform_device *pdev)
{
	struct mxc_audio_platform_data *plat = pdev->dev.platform_data;
	struct imx_priv *priv = &card_priv;
	int ret = 0;

	priv->Data_40 = clk_get(NULL, "clko_clk");

	if (priv->Data_40 > -0x1000)
	{
		printk("<3>can't get CLKO clock.\n");
		return priv->Data_40;
	}

	priv->pdev = pdev;

	imx_audmux_config(plat->src_port, plat->ext_port);

	if (plat->init && plat->init()) {
		ret = -EINVAL;
		return ret;
	}

	priv->sysclk = plat->sysclk;

	return ret;
}

static int __devexit imx_wm8960_remove(struct platform_device *pdev)
{
	struct mxc_audio_platform_data *plat = pdev->dev.platform_data;

	if (plat->finit)
		plat->finit();

	clk_disable(card_priv.Data_40);
	clk_put(card_priv.Data_40);

	return 0;
}

static struct platform_driver imx_wm8960_driver = {
	.probe = imx_wm8960_probe,
	.remove = imx_wm8960_remove,
	.driver = {
		   .name = "imx-wm8960",
		   .owner = THIS_MODULE,
		   },
};

static struct platform_device *imx_snd_device; //C0922180 +0x2c

static int __init imx_asoc_init(void)
{
	int ret;

	ret = platform_driver_register(&imx_wm8960_driver);
	if (ret < 0)
		goto exit;

#if 0
	if (machine_is_mx6q_sabresd())
		imx_dai[0].codec_name = "wm8962.0-001a";
	else if (machine_is_mx6sl_arm2() | machine_is_mx6sl_evk())
		imx_dai[0].codec_name = "wm8962.1-001a";
#endif

	imx_snd_device = platform_device_alloc("soc-audio", 5);
	if (!imx_snd_device)
		goto err_device_alloc;

	platform_set_drvdata(imx_snd_device, &snd_soc_card_imx);

	ret = platform_device_add(imx_snd_device);

	if (0 == ret)
		goto exit;

	platform_device_put(imx_snd_device);

err_device_alloc:
	platform_driver_unregister(&imx_wm8960_driver);
exit:
	return ret;
}

static void __exit imx_asoc_exit(void)
{
	platform_driver_unregister(&imx_wm8960_driver);
	platform_device_unregister(imx_snd_device);
}

module_init(imx_asoc_init);
module_exit(imx_asoc_exit);

/* Module information */
MODULE_DESCRIPTION("ALSA SoC imx wm8960");
MODULE_LICENSE("GPL");
