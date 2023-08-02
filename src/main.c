/*
 * Copyright (c) 2019-2020 Peter Bigot Consulting, LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/rtc/maxim_ds3231.h>
#include <zephyr/drivers/eeprom.h>

#define GET_TIME_DELAY_MS	K_MSEC(1000)
#define EEPROM_NODE DT_NODELABEL(eeprom_57)

/*
 * Get a device structure from a devicetree node with alias eeprom-0
 */
static const struct device *get_eeprom_device(void)
{
	const struct device *const dev = DEVICE_DT_GET_ONE(atmel_at24);

	if (!device_is_ready(dev)) {
		printk("\nError: Device \"%s\" is not ready; "
		       "check the driver initialization logs for errors.\n",
		       dev->name);
		return NULL;
	}

	printk("Found EEPROM device \"%s\"\n", dev->name);
	return dev;
}

/* Format times as: YYYY-MM-DD HH:MM:SS DOW DOY */
static const char *format_time(time_t time,
			       long nsec)
{
	static char buf[64];
	char *bp = buf;
	char *const bpe = bp + sizeof(buf);
	struct tm tv;
	struct tm *tp = gmtime_r(&time, &tv);

	bp += strftime(bp, bpe - bp, "%Y-%m-%d %H:%M:%S", tp);
	if (nsec >= 0) {
		bp += snprintf(bp, bpe - bp, ".%09lu", nsec);
	}
	bp += strftime(bp, bpe - bp, " %a %j", tp);
	return buf;
}

int main(void)
{
	uint32_t now = 0;
	const struct device *const ds3231 = DEVICE_DT_GET_ONE(maxim_ds3231);
	const struct device *eeprom = get_eeprom_device();

	if (!device_is_ready(ds3231)) {
		printk("%s: device not ready.\n", ds3231->name);
		return 0;
	}

	int rc = maxim_ds3231_stat_update(ds3231, 0, MAXIM_DS3231_REG_STAT_OSF);

	if (rc >= 0) {
		printk("DS3231 has%s experienced an oscillator fault\n",
		       (rc & MAXIM_DS3231_REG_STAT_OSF) ? "" : " not");
	} else {
		printk("DS3231 stat fetch failed: %d\n", rc);
		return 0;
	}



	for (;;)
	{
		counter_get_value(ds3231, &now);
		printk("Now %u: %s\n", now, format_time(now, -1));
		k_sleep(GET_TIME_DELAY_MS);
	}
	//Never reached	
	k_sleep(K_FOREVER);
	return 0;
}
