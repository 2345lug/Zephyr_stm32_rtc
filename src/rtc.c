#include "rtc.h"

K_MUTEX_DEFINE(time_mutex);

/**
       * Function for format times as: YYYY-MM-DD HH:MM:SS DOW DOY
       * @param time Current time.
       * @param nsec Current time ns value.
       * @return Pointer to formatted string
       */
const char *format_time(time_t time, long nsec)
{
	static char buf[64];
	char *bp = buf;
	char *const bpe = bp + sizeof(buf);
	struct tm tv;
	struct tm *tp = gmtime_r(&time, &tv);

	bp += strftime(bp, bpe - bp, "%Y-%m-%d %H:%M:%S", tp);
	if (nsec >= 0) 
    {
		bp += snprintf(bp, bpe - bp, ".%09lu", nsec);
	}
	bp += strftime(bp, bpe - bp, " %a %j", tp);
	return buf;
}

/**
       * Function for check RTC device status
       * @param rtc_device Pointer to RTC device structure.
       * @return Result of operation
       */
int rtc_stat_update (const struct device * rtc_device)
{
    int rc = 0;

	if (!device_is_ready(rtc_device)) {
		printk("%s: device not ready.\n", rtc_device->name);
		return rc;
	}

    rc = maxim_ds3231_stat_update(rtc_device, 0, MAXIM_DS3231_REG_STAT_OSF);

	if (rc >= 0) 
    {
		printk("DS3231 has%s experienced an oscillator fault\n",
		       (rc & MAXIM_DS3231_REG_STAT_OSF) ? "" : " not");
	} else 
    {
		printk("DS3231 stat fetch failed: %d\n", rc);
		
	}

    return rc;
}

/**
       * Function for get current time value from RTC
       * @return Current time
       */
uint32_t get_current_time(const struct device * rtc_device)
{
    uint32_t current_time = 0;
    counter_get_value(rtc_device, &current_time);
    return current_time;
}
