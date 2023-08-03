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
#include <zephyr/shell/shell.h>

#include "eeprom_memory.h"

#define GET_TIME_PERIOD_MS	K_MSEC(1000)

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

static int cmd_get_time(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "pong");

	return 0;
}

static int cmd_get_shutdown_time(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	uint32_t shutdown_time = get_previous_shutdown_time();
	shell_print(sh, "Previous shutdown time is %u: %s\n", shutdown_time, format_time(shutdown_time, -1));

	return 0;
}

int main(void)
{
	uint32_t now = 0;	
	size_t eeprom_size = 0;
	int rc = 0;
	const struct device *const ds3231 = DEVICE_DT_GET_ONE(maxim_ds3231);
	const struct device *eeprom = get_eeprom_device();

	eeprom_size = eeprom_get_size(eeprom);
	printk("Using eeprom with size of: %zu.\n", eeprom_size);

	SHELL_STATIC_SUBCMD_SET_CREATE(rtc_time,
		SHELL_CMD(get_time, NULL, "Get current time.", cmd_get_time),
		SHELL_CMD(get_shutdown_time, NULL, "Get last shutdown_time.", cmd_get_shutdown_time),
		SHELL_SUBCMD_SET_END /* Array terminated. */
	);

	SHELL_CMD_REGISTER(rtc, &rtc_time, "RTC control commands", NULL);

	if (!device_is_ready(ds3231)) {
		printk("%s: device not ready.\n", ds3231->name);
		return 0;
	}

	rc = maxim_ds3231_stat_update(ds3231, 0, MAXIM_DS3231_REG_STAT_OSF);

	if (rc >= 0) {
		printk("DS3231 has%s experienced an oscillator fault\n",
		       (rc & MAXIM_DS3231_REG_STAT_OSF) ? "" : " not");
	} else {
		printk("DS3231 stat fetch failed: %d\n", rc);
		return 0;
	}

	counter_get_value(ds3231, &now);
	compare_previous_time(eeprom, now);

	for (;;)
	{
		counter_get_value(ds3231, &now);
		#if 0
		printk("Now %u: %s\n", now, format_time(now, -1));
		#endif
		set_previous_time(eeprom, now);
		k_sleep(GET_TIME_PERIOD_MS);
	}
	//Never reached	
	k_sleep(K_FOREVER);
	return 0;
}
