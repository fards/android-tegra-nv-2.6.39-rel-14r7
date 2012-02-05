/*
 * arch/arm/mach-tegra/board-smba1007-jack.c
 *
 * Copyright (c) 2011, NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/platform_device.h>
#include <sound/jack.h>
#include <sound/soc.h>
#include <mach/gpio.h>
#include <linux/regulator/consumer.h>
#include <mach/audio.h>

#include "gpio-names.h"
#include "board-smba1007.h"

static struct wired_jack_conf smba1007_wr_jack_conf = {
	.hp_det_n = SMBA1007_HP_DETECT,
	.en_mic_ext = -1,
	.en_mic_int = -1,
	.en_spkr = ALC5623_GP0,
	.cdc_irq = -1,
//	.spkr_amp_reg = "avdd_amp"
};

static struct platform_device smba1007_hs_jack_device = {
	.name = "tegra_wired_jack",
	.id = -1,
	.dev = {
		.platform_data = &smba1007_wr_jack_conf,
	},
};

int __init smba1007_jack_register_devices(void)
{
	int ret;

	tegra_gpio_enable(smba1007_wr_jack_conf.hp_det_n);
//	tegra_gpio_enable(smba1007_wr_jack_conf.en_mic_int);
//	tegra_gpio_enable(smba1007_wr_jack_conf.en_mic_ext);
//	tegra_gpio_enable(smba1007_wr_jack_conf.cdc_irq);

	ret = platform_device_register(&smba1007_hs_jack_device);
	return ret;
}
