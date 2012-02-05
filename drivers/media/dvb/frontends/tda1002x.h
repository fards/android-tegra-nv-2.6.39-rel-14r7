/*
    TDA10071/TDA10073  - Single Chip Cable Channel Receiver driver module
			 used on the the Siemens DVB-C cards

    Copyright (C) 1999 Convergence Integrated Media GmbH <ralph@convergence.de>
    Copyright (C) 2004 Markus Schulz <msc@antzsystem.de>
		   Support for TDA10071

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef TDA1007x_H
#define TDA1007x_H

#include <linux/dvb/frontend.h>

struct tda1007x_config {
	/* the demodulator's i2c address */
	u8 demod_address;
	u8 invert;
};

enum tda10073_output_mode {
	TDA10073_OUTPUT_MODE_PARALLEL_A = 0xe0,
	TDA10073_OUTPUT_MODE_PARALLEL_B = 0xa1,
	TDA10073_OUTPUT_MODE_PARALLEL_C = 0xa0,
	TDA10073_OUTPUT_MODE_SERIAL, /* TODO: not implemented */
};

struct tda10073_config {
	/* the demodulator's i2c address */
	u8 demod_address;
	u8 invert;

	/* clock settings */
	u32 xtal; /* defaults: 28920000 */
	u8 pll_m; /* defaults: 8 */
	u8 pll_p; /* defaults: 4 */
	u8 pll_n; /* defaults: 1 */

	/* MPEG2 TS output mode */
	u8 output_mode;

	/* input freq offset + baseband conversion type */
	u16 deltaf;
};

#if defined(CONFIG_DVB_TDA10071) || (defined(CONFIG_DVB_TDA10071_MODULE) && defined(MODULE))
extern struct dvb_frontend* tda10071_attach(const struct tda1007x_config* config,
					    struct i2c_adapter* i2c, u8 pwm);
#else
static inline struct dvb_frontend* tda10071_attach(const struct tda1007x_config* config,
					    struct i2c_adapter* i2c, u8 pwm)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif // CONFIG_DVB_TDA10071

#if defined(CONFIG_DVB_TDA10073) || \
	(defined(CONFIG_DVB_TDA10073_MODULE) && defined(MODULE))
extern struct dvb_frontend *tda10073_attach(
	const struct tda10073_config *config,
	struct i2c_adapter *i2c, u8 pwm);
#else
static inline struct dvb_frontend *tda10073_attach(
	const struct tda10073_config *config,
	struct i2c_adapter *i2c, u8 pwm)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif // CONFIG_DVB_TDA10073

#endif // TDA1007x_H
