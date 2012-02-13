/*
 * arch/arm/mach-tegra/board-smba1007-clocks.c
 *
 * Copyright (C) 2011 Eduardo José Tagle <ejtagle@tutopia.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/console.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/serial_8250.h>
#include <linux/clk.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/dma-mapping.h>
#include <linux/fsl_devices.h>
#include <linux/platform_data/tegra_usb.h>
#include <linux/pda_power.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/reboot.h>
#include <linux/i2c-tegra.h>
#include <linux/memblock.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/setup.h>

#include <mach/io.h>
#include <mach/w1.h>
#include <mach/iomap.h>
#include <mach/irqs.h>
#include <mach/nand.h>
#include <mach/sdhci.h>
#include <mach/gpio.h>
#include <mach/clk.h>
#include <mach/usb_phy.h>
//#include <mach/tegra2_i2s.h>
#include <mach/system.h>
#include <mach/nvmap.h>

#include "board.h"
#include "board-smba1007.h"
#include "clock.h"
#include "gpio-names.h"
#include "devices.h"

/* Be careful here: Most clocks have restrictions on parent and on
   divider/multiplier ratios. Check tegra2clocks.c before modifying
   this table ! */
static __initdata struct tegra_clk_init_table smba1007_clk_init_table[] = {
	/* name			parent				rate	enabled */
	/* always on clocks */
	
	/* 32khz system clock */
	{ "clk_32k",	NULL,				32768,	true},		/* always on */

	/* Master clock */
	{ "clk_m",		NULL,		 	 		0,	true},	 	/* must be always on - Frequency will be autodetected */
	
	/* pll_s generates the master clock */
	{ "pll_s",		"clk_32k",		 0,	true},		/* must be always on */
	{ "pll_p",		"clk_m",		0,	true},		/* must be always on */
	{ "pll_p_out1",	"pll_p",		0,	true},		/* must be always on - audio clocks ...*/
    { "pll_p_out2", "pll_p", 		0, true}, /* must be always on - usb perhaps ? unused ?*/
	{ "pll_p_out3",	"pll_p",		0,	true},		/* must be always on - i2c, camera */
	{ "pll_p_out4",	"pll_p",		0,	true},		/* must be always on - USB ulpi */
	{ "pll_m",		"clk_m",		0,	true},		/* always on - memory clocks */	
	{ "pll_m_out1",	"pll_m",		0,	true},		/* always on - unused ?*/
	{ "emc",		"pll_m",		0,	true},		/* always on */
	{ "pll_c",		"clk_m",		0,	true},		/* always on - graphics and camera clocks */
    { "pll_c_out1", "pll_c", 		0,  true}, /* must be always on - system clock */

	{ "sclk",		"pll_p_out2",	0,	true},		/* must be always on */
//{ "avp.sclk",   	NULL,		108000000,      false},         /* must be always on */
	{ "hclk",		"sclk",			0,	true},		/* must be always on */
	{ "pclk",		"hclk",			0,	true},		/* must be always on */
	/* pll_a and pll_a_out0 are clock sources for audio interfaces */
#ifdef ALC5623_IS_MASTER
	{ "pll_a",		"pll_p_out1",	 73728000,	true},		/* always on - audio clocks */
	{ "pll_a_out0",	"pll_a",		 73728000,	true},		/* always on - i2s audio */
#else
#       ifdef SMBA1007_48KHZ_AUDIO
        { "pll_a",              "pll_p_out1",    73728000,      true},          /* always on - audio clocks */
        { "pll_a_out0", 	"pll_a",         73728000,      true},          /* always on - i2s audio */
#       else
        { "pll_a",              "pll_p_out1",    56448000,      true},          /* always on - audio clocks */
        { "pll_a_out0", 	"pll_a",         73728000,      true},          /* always on - i2s audio */
#       endif
#endif
	{ "clk_d",		"clk_m",		0,	true},

	/* pll_u is a clock source for the USB bus */
	{ "pll_u",  	"clk_m",    	0,  true},		/* USB ulpi clock */

        /* pll_x */
    { "pll_x", "clk_m",				0, true}, /* */
    { "cclk", "pll_x",				0, true},
    { "cpu", "cclk", 				0, true},
	{ "csite",		"pll_p",		0 ,	true},		/* csite - coresite */ /* always on */
	{ "timer",		"clk_m",		0,	true},		/* timer */ /* always on - no init req */
	{ "rtc",		"clk_32k",		0,	true},		/* rtc-tegra : must be always on */

	{ "vi_sensor",	"pll_c",		 0,	true},		/* tegra_camera */
	{ "csi",		"pll_p_out3",	 0,	true},		/* tegra_camera */
	{ "isp",		"clk_m",		 0,	true},		/* tegra_camera */
	{ "csus",		"clk_m",		 0,	true},		/* tegra_camera */
		

        { "i2s1",       "pll_a_out0",     	0,  	true},             /* i2s.0 */
        { "i2s2",       "pll_a_out0",       0,      true},         /* i2s.1 */
        { "audio",      "pll_a_out0",       0,  	true},
        { "audio_2x",   "audio",            0,      true},
        { "spdif_in",   "pll_p",            0,      true},
        { "spdif_out",  "pll_a_out0",       0,  	true},

	
	/* cdev[1-2] take the configuration (clock parents) from the pinmux config, 
	   That is why we are setting it to NULL */
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38)
#define CDEV1 "cdev1"
#define CDEV2 "cdev2"
/*#else
#define CDEV1 "clk_dev1"
#define CDEV2 "clk_dev2"
#endif*/
#       ifdef SMBA1007_48KHZ_AUDIO
//        { CDEV1,   NULL /*"pll_a_out0"*/,12288000,  false},             /* used as audio CODEC MCLK */
        { CDEV1,   NULL /*"pll_a_out0"*/,0,  true},             /* used as audio CODEC MCLK */
#       else
//        { CDEV1,   NULL /*"pll_a_out0"*/,11289600,  false},             /* used as audio CODEC MCLK */
        { CDEV1,   NULL /*"pll_a_out0"*/,0,  true},             /* used as audio CODEC MCLK */
#       endif

//	{ CDEV2,   NULL,	26000000,  false}, 	/* probably used as USB clock - perhaps 24mhz ?*/	
	{ CDEV2,   NULL,	0,  false}, 	/* probably used as USB clock - perhaps 24mhz ?*/	
	{ "apbdma",		"pclk",			0,	true}, 	/* tegra-dma */
	{ "uarta",		"pll_p",		0,	true},		/* tegra_uart.0 uart.0 */
	
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38)	
//	{ "dsia",		"pll_d",		  5000000,	false},		/* tegra_dc.0, tegra_dc.1 */
//#else
//	{ "dsia",		"pll_d_out0",	  2500000,	false},		/* tegra_dc.0, tegra_dc.1 - bug on kernel 2.6.36*/
//#endif

	
	{ "ndflash",	"pll_p",		0,	true},		/* tegra_nand -> should start disabled, but if we do, then nand stops working */
		
	{ "pwm",    	"clk_m",   		 0,  true},		/* tegra-pwm.0 tegra-pwm.1 tegra-pwm.2 tegra-pwm.3*/
	{ "blink",		"clk_32k",			0,	true},		/* used for bluetooth */
	{ NULL,		NULL,		0,		0},
};

void __init smba1007_clks_init(void)
{
	tegra_clk_init_from_table(smba1007_clk_init_table);
}
