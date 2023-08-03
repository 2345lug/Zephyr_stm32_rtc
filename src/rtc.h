#ifndef RTC_H
#define RTC_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/rtc/maxim_ds3231.h>

const char *format_time(time_t time, long nsec);
int rtc_stat_update (const struct device * rtc_device);
uint32_t get_current_time(const struct device * rtc_device);
int set_current_time (const struct device * rtc_device, time_t current_time);

#endif