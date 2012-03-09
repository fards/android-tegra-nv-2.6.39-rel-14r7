/*
 * arch/arm/mach-tegra/board-smba1007-sdhci.c
 *
 * Copyright (C) 2011 Eduardo José Tagle <ejtagle@tutopia.com> 
 * Copyright (C) 2010 Google, Inc.
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
#define DEBUG 1
#include <linux/resource.h>
#include <linux/platform_device.h>
#include <linux/wlan_plat.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/version.h>

#include <asm/mach-types.h>
#include <mach/irqs.h>
#include <mach/iomap.h>
#include <mach/sdhci.h>
#include <mach/pinmux.h>

#include "gpio-names.h"
#include "devices.h"
#include "board-smba1007.h"

#define SMBA1007_WLAN_WOW	TEGRA_GPIO_PS0

static void (*wlan_status_cb)(int card_present, void *dev_id);
static void *wlan_status_cb_devid;
static int smba1007_wlan_cd = 0; /* WIFI virtual 'card detect' status */

static int smba1007_wifi_status_register(void (*callback)(int , void *), void *);
//static struct clk *wifi_32k_clk;

static int smba1007_wifi_reset(int on);
static int smba1007_wifi_power(int on);
static int smba1007_wifi_set_carddetect(int val);

static struct wifi_platform_data smba1007_wifi_control = {
        .set_power      = smba1007_wifi_power,
        .set_reset      = smba1007_wifi_reset,
        .set_carddetect = smba1007_wifi_set_carddetect,
};

static struct resource wifi_resource[] = {
	[0] = {
		.name  = "bcm4329_wlan_irq",
		.start = TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PS0),
		.end   = TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PS0),
		.flags = IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL | IORESOURCE_IRQ_SHAREABLE,
	},
};

static struct platform_device smba1007_wifi_device = {
        .name           = "bcmdhd_wlan",
        .id             = 1,
	.num_resources  = 1,
	.resource	= wifi_resource,
        .dev            = {
        .platform_data = &smba1007_wifi_control,
        },
};

static struct resource sdhci_resource0[] = {
	[0] = {
		.start	= INT_SDMMC1,
		.end	= INT_SDMMC1,
		.flags	= IORESOURCE_IRQ,
	},
	[1] = {
		.start	= TEGRA_SDMMC1_BASE,
		.end	= TEGRA_SDMMC1_BASE + TEGRA_SDMMC1_SIZE-1,
		.flags	= IORESOURCE_MEM,
	},
};

/* 2.6.36 version has a hook to check card status. Use it */
static unsigned int smba1007_wlan_status(struct device *dev)
{
	return smba1007_wlan_cd;
}

static int smba1007_wifi_status_register(
		void (*callback)(int card_present, void *dev_id),
		void *dev_id)
{
	if (wlan_status_cb)
		return -EAGAIN;
	wlan_status_cb = callback;
	wlan_status_cb_devid = dev_id;
	return 0;
} 
static struct embedded_sdio_data embedded_sdio_data0 = {
        .cccr   = {
                .sdio_vsn       = 2,
                .multi_block    = 1,
                .low_speed      = 0,
                .wide_bus       = 0,
                .high_power     = 1,
                .high_speed     = 1,
        },
        .cis  = {
                .vendor         = 0x02d0,
                .device         = 0x4329,
        },
};

struct tegra_sdhci_platform_data smba1007_wlan_data = {
//        .clk_id = NULL,
//        .force_hs = 0,
	.mmc_data = {
        	.register_status_notify = smba1007_wifi_status_register,
		.embedded_sdio = &embedded_sdio_data0,
		.built_in = 1,
		.status = smba1007_wlan_status,
	},
	.cd_gpio = -1,
	.wp_gpio = -1,
	.power_gpio = -1,
	.has_no_vreg = 1,
};

/* Used to set the virtual CD of wifi adapter */
int smba1007_wifi_set_carddetect(int val)
{
	/* Only if a change is detected */
	if (smba1007_wlan_cd != val) {
	
		/* Store new card 'detect' */
		smba1007_wlan_cd = val;
		
		/* Let the SDIO infrastructure know about the change */
		if (wlan_status_cb) {
			wlan_status_cb(val, wlan_status_cb_devid);
		} else
			pr_info("%s: Nobody to notify\n", __func__);
	}
	return 0;
}

static int smba1007_wifi_power(int on)
{
        pr_debug("%s: %d\n", __func__, on);
	//mdelay(100);
		smba1007_bt_wifi_gpio_set(on);
	//	mdelay(100);
        gpio_set_value(SMBA1007_WLAN_RESET, on);
        mdelay(200);

       

        return 0;
}


static int smba1007_wifi_reset(int on)
{
	gpio_set_value(SMBA1007_WLAN_RESET, !on);
        pr_debug("%s: %d\n", __func__, on);
//	pr_debug("%s: do nothing, on = %d\n", __func__, on);
        return 0;
}


static struct tegra_sdhci_platform_data tegra_sdhci_platform_data2 = {
	.cd_gpio = TEGRA_GPIO_PI5,
	.wp_gpio = TEGRA_GPIO_PH1,
	.power_gpio = TEGRA_GPIO_PT3,
};

static struct tegra_sdhci_platform_data tegra_sdhci_platform_data3 = {
	.cd_gpio = SMBA1007_SDHC_CD,
	.wp_gpio = -1,
	.power_gpio = SMBA1007_SDHC_POWER,
	.has_no_vreg = 1,
};

static struct tegra_sdhci_platform_data tegra_sdhci_platform_data4 = {
	.cd_gpio = -1,
	.wp_gpio = -1,
	.power_gpio = -1,
	.has_no_vreg = 1,
	.is_8bit = 1,
	.max_clk_limit = 52000000,
	.mmc_data = {
		.built_in = 1,
	},
	
};



static struct platform_device *smba1007_sdhci_devices[] __initdata = {
	&tegra_sdhci_device1,
//	&tegra_sdhci_device2,
//have to init these out of order so that the eMMC card is registered first
	&tegra_sdhci_device4,
	&tegra_sdhci_device3,
};

static int __init smba1007_wifi_init(void)
{
        int gpio_pwr, gpio_rst;

//	if (!machine_is_harmony())
	//	return 0;

        /* WLAN - Power up (low) and Reset (low) */
        gpio_pwr = gpio_request(TEGRA_GPIO_PK5, "wlan_power");
        gpio_rst = gpio_request(TEGRA_GPIO_PK6, "wlan_reset");
        if (gpio_pwr < 0 || gpio_rst < 0)
                pr_warning("Unable to get gpio for WLAN Power and Reset\n");
        else {

		tegra_gpio_enable(TEGRA_GPIO_PK5);
		tegra_gpio_enable(TEGRA_GPIO_PK6);
                /* toggle in this order as per spec */
                gpio_direction_output(TEGRA_GPIO_PK5, 0);
                gpio_direction_output(TEGRA_GPIO_PK6, 0);
		udelay(5);
                gpio_direction_output(TEGRA_GPIO_PK5, 1);
                gpio_direction_output(TEGRA_GPIO_PK6, 1);
        }

	return 0;
}


/* Register sdhci devices */
int __init smba1007_sdhci_register_devices(void)
{
	int ret=0;
	/* Plug in platform data */
	tegra_sdhci_device1.dev.platform_data = &smba1007_wlan_data;
	tegra_sdhci_device2.dev.platform_data = &tegra_sdhci_platform_data2;
	tegra_sdhci_device3.dev.platform_data = &tegra_sdhci_platform_data3;
	tegra_sdhci_device4.dev.platform_data = &tegra_sdhci_platform_data4;

	ret = platform_add_devices(smba1007_sdhci_devices, ARRAY_SIZE(smba1007_sdhci_devices));
	smba1007_wifi_init();
	return ret;

}
