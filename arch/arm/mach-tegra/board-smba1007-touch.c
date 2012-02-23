/*
 * arch/arm/mach-tegra/board-smba1007-touch.c
 *
 * Copyright (C) 2011 Jens Andersen <jens.andersen@gmail.com
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
 
#include <linux/resource.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c/at168_ts.h>
#include <asm/mach-types.h>
#include <mach/irqs.h>
#include <mach/iomap.h>
#include <mach/pinmux.h>
#include <linux/interrupt.h>
#include <linux/input.h>

#include "board-smba1007.h"
#include "gpio-names.h"

struct at168_i2c_ts_platform_data at168_pdata = {
	.gpio_reset = SMBA1007_TS_RESET,
	.gpio_power = SMBA1007_TS_IRQ,
};

static struct i2c_board_info __initdata smba1007_i2c_bus0_touch_info_at168[] = {
	{
		I2C_BOARD_INFO("at168_touch", 0x5c),
		.irq = TEGRA_GPIO_TO_IRQ(SMBA1007_TS_IRQ),
		.platform_data = &at168_pdata,
	},
};


int __init smba1007_touch_register_devices(void)
{	 	tegra_gpio_enable(TEGRA_GPIO_PJ7);
  		gpio_request(TEGRA_GPIO_PJ7, "at168_touch");
  		gpio_direction_input(TEGRA_GPIO_PJ7);
	i2c_register_board_info(0, smba1007_i2c_bus0_touch_info_at168, 1);

	return 0;
}
