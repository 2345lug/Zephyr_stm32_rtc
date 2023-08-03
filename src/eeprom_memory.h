#ifndef EEPROM_MEMORY_H
#define EEPROM_MEMORY_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>

const struct device *get_eeprom_device(void);
uint32_t compare_previous_time (const struct device * eeprom_device, uint32_t now);
int set_previous_time (const struct device * eeprom_device, uint32_t now);
uint32_t get_previous_shutdown_time (void);

#endif